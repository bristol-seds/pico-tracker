'''Wrapper for tmain.c

Generated with:
test/ctypesgen/ctypesgen.py -o test/main.py --cpp=arm-none-eabi-gcc -E -DCTYPESGEN  -g3 -ggdb -O1 -Wall -Wextra  -std=gnu99 -ffunction-sections -fdata-sections -mcpu=cortex-m0plus -mthumb -DSAMD20E18 -D__SAMD20E18__ -Iinc/ -Ichip/ -Ichip/cmsis/ -Isamd20/ -Isamd20/component/ -Itest/tc/ test/tmain.c test/tc/osc8m_calib.h test/tc/mem_write_page.h test/tc/location_aprs_file.h test/tc/barometric_altitude.h test/tc/backlog_write_read.h test/tc/mem_write_all.h test/tc/times_two.h test/tc/mem_erase_all.h test/tc/location_telemetry.h test/tc/pressure_temperature.h test/tc/gps_poll.h test/tc/epoch_from_time.h test/tc/gps_baud_error.h test/tc/crc32_gen_buf.h test/tc/analogue_read.h test/tc/read_rocket.h test/tc/adc_battery_solar_read.h test/tc/location_aprs.h test/tc/backlog_read.h test/tc/thermistor_equation.h

Do not modify this file.
'''

__docformat__ =  'restructuredtext'

# Begin preamble

import ctypes, os, sys
from ctypes import *

_int_types = (c_int16, c_int32)
if hasattr(ctypes, 'c_int64'):
    # Some builds of ctypes apparently do not have c_int64
    # defined; it's a pretty good bet that these builds do not
    # have 64-bit pointers.
    _int_types += (c_int64,)
for t in _int_types:
    if sizeof(t) == sizeof(c_size_t):
        c_ptrdiff_t = t
del t
del _int_types

class c_void(Structure):
    # c_void_p is a buggy return type, converting to int, so
    # POINTER(None) == c_void_p is actually written as
    # POINTER(c_void), so it can be treated as a real pointer.
    _fields_ = [('dummy', c_int)]

def POINTER(obj):
    p = ctypes.POINTER(obj)

    # Convert None to a real NULL pointer to work around bugs
    # in how ctypes handles None on 64-bit platforms
    if not isinstance(p.from_param, classmethod):
        def from_param(cls, x):
            if x is None:
                return cls()
            else:
                return x
        p.from_param = classmethod(from_param)

    return p

class UserString:
    def __init__(self, seq):
        if isinstance(seq, basestring):
            self.data = seq
        elif isinstance(seq, UserString):
            self.data = seq.data[:]
        else:
            self.data = str(seq)
    def __str__(self): return str(self.data)
    def __repr__(self): return repr(self.data)
    def __int__(self): return int(self.data)
    def __long__(self): return long(self.data)
    def __float__(self): return float(self.data)
    def __complex__(self): return complex(self.data)
    def __hash__(self): return hash(self.data)

    def __cmp__(self, string):
        if isinstance(string, UserString):
            return cmp(self.data, string.data)
        else:
            return cmp(self.data, string)
    def __contains__(self, char):
        return char in self.data

    def __len__(self): return len(self.data)
    def __getitem__(self, index): return self.__class__(self.data[index])
    def __getslice__(self, start, end):
        start = max(start, 0); end = max(end, 0)
        return self.__class__(self.data[start:end])

    def __add__(self, other):
        if isinstance(other, UserString):
            return self.__class__(self.data + other.data)
        elif isinstance(other, basestring):
            return self.__class__(self.data + other)
        else:
            return self.__class__(self.data + str(other))
    def __radd__(self, other):
        if isinstance(other, basestring):
            return self.__class__(other + self.data)
        else:
            return self.__class__(str(other) + self.data)
    def __mul__(self, n):
        return self.__class__(self.data*n)
    __rmul__ = __mul__
    def __mod__(self, args):
        return self.__class__(self.data % args)

    # the following methods are defined in alphabetical order:
    def capitalize(self): return self.__class__(self.data.capitalize())
    def center(self, width, *args):
        return self.__class__(self.data.center(width, *args))
    def count(self, sub, start=0, end=sys.maxint):
        return self.data.count(sub, start, end)
    def decode(self, encoding=None, errors=None): # XXX improve this?
        if encoding:
            if errors:
                return self.__class__(self.data.decode(encoding, errors))
            else:
                return self.__class__(self.data.decode(encoding))
        else:
            return self.__class__(self.data.decode())
    def encode(self, encoding=None, errors=None): # XXX improve this?
        if encoding:
            if errors:
                return self.__class__(self.data.encode(encoding, errors))
            else:
                return self.__class__(self.data.encode(encoding))
        else:
            return self.__class__(self.data.encode())
    def endswith(self, suffix, start=0, end=sys.maxint):
        return self.data.endswith(suffix, start, end)
    def expandtabs(self, tabsize=8):
        return self.__class__(self.data.expandtabs(tabsize))
    def find(self, sub, start=0, end=sys.maxint):
        return self.data.find(sub, start, end)
    def index(self, sub, start=0, end=sys.maxint):
        return self.data.index(sub, start, end)
    def isalpha(self): return self.data.isalpha()
    def isalnum(self): return self.data.isalnum()
    def isdecimal(self): return self.data.isdecimal()
    def isdigit(self): return self.data.isdigit()
    def islower(self): return self.data.islower()
    def isnumeric(self): return self.data.isnumeric()
    def isspace(self): return self.data.isspace()
    def istitle(self): return self.data.istitle()
    def isupper(self): return self.data.isupper()
    def join(self, seq): return self.data.join(seq)
    def ljust(self, width, *args):
        return self.__class__(self.data.ljust(width, *args))
    def lower(self): return self.__class__(self.data.lower())
    def lstrip(self, chars=None): return self.__class__(self.data.lstrip(chars))
    def partition(self, sep):
        return self.data.partition(sep)
    def replace(self, old, new, maxsplit=-1):
        return self.__class__(self.data.replace(old, new, maxsplit))
    def rfind(self, sub, start=0, end=sys.maxint):
        return self.data.rfind(sub, start, end)
    def rindex(self, sub, start=0, end=sys.maxint):
        return self.data.rindex(sub, start, end)
    def rjust(self, width, *args):
        return self.__class__(self.data.rjust(width, *args))
    def rpartition(self, sep):
        return self.data.rpartition(sep)
    def rstrip(self, chars=None): return self.__class__(self.data.rstrip(chars))
    def split(self, sep=None, maxsplit=-1):
        return self.data.split(sep, maxsplit)
    def rsplit(self, sep=None, maxsplit=-1):
        return self.data.rsplit(sep, maxsplit)
    def splitlines(self, keepends=0): return self.data.splitlines(keepends)
    def startswith(self, prefix, start=0, end=sys.maxint):
        return self.data.startswith(prefix, start, end)
    def strip(self, chars=None): return self.__class__(self.data.strip(chars))
    def swapcase(self): return self.__class__(self.data.swapcase())
    def title(self): return self.__class__(self.data.title())
    def translate(self, *args):
        return self.__class__(self.data.translate(*args))
    def upper(self): return self.__class__(self.data.upper())
    def zfill(self, width): return self.__class__(self.data.zfill(width))

class MutableString(UserString):
    """mutable string objects

    Python strings are immutable objects.  This has the advantage, that
    strings may be used as dictionary keys.  If this property isn't needed
    and you insist on changing string values in place instead, you may cheat
    and use MutableString.

    But the purpose of this class is an educational one: to prevent
    people from inventing their own mutable string class derived
    from UserString and than forget thereby to remove (override) the
    __hash__ method inherited from UserString.  This would lead to
    errors that would be very hard to track down.

    A faster and better solution is to rewrite your program using lists."""
    def __init__(self, string=""):
        self.data = string
    def __hash__(self):
        raise TypeError("unhashable type (it is mutable)")
    def __setitem__(self, index, sub):
        if index < 0:
            index += len(self.data)
        if index < 0 or index >= len(self.data): raise IndexError
        self.data = self.data[:index] + sub + self.data[index+1:]
    def __delitem__(self, index):
        if index < 0:
            index += len(self.data)
        if index < 0 or index >= len(self.data): raise IndexError
        self.data = self.data[:index] + self.data[index+1:]
    def __setslice__(self, start, end, sub):
        start = max(start, 0); end = max(end, 0)
        if isinstance(sub, UserString):
            self.data = self.data[:start]+sub.data+self.data[end:]
        elif isinstance(sub, basestring):
            self.data = self.data[:start]+sub+self.data[end:]
        else:
            self.data =  self.data[:start]+str(sub)+self.data[end:]
    def __delslice__(self, start, end):
        start = max(start, 0); end = max(end, 0)
        self.data = self.data[:start] + self.data[end:]
    def immutable(self):
        return UserString(self.data)
    def __iadd__(self, other):
        if isinstance(other, UserString):
            self.data += other.data
        elif isinstance(other, basestring):
            self.data += other
        else:
            self.data += str(other)
        return self
    def __imul__(self, n):
        self.data *= n
        return self

class String(MutableString, Union):

    _fields_ = [('raw', POINTER(c_char)),
                ('data', c_char_p)]

    def __init__(self, obj=""):
        if isinstance(obj, (str, unicode, UserString)):
            self.data = str(obj)
        else:
            self.raw = obj

    def __len__(self):
        return self.data and len(self.data) or 0

    def from_param(cls, obj):
        # Convert None or 0
        if obj is None or obj == 0:
            return cls(POINTER(c_char)())

        # Convert from String
        elif isinstance(obj, String):
            return obj

        # Convert from str
        elif isinstance(obj, str):
            return cls(obj)

        # Convert from c_char_p
        elif isinstance(obj, c_char_p):
            return obj

        # Convert from POINTER(c_char)
        elif isinstance(obj, POINTER(c_char)):
            return obj

        # Convert from raw pointer
        elif isinstance(obj, int):
            return cls(cast(obj, POINTER(c_char)))

        # Convert from object
        else:
            return String.from_param(obj._as_parameter_)
    from_param = classmethod(from_param)

def ReturnString(obj, func=None, arguments=None):
    return String.from_param(obj)

# As of ctypes 1.0, ctypes does not support custom error-checking
# functions on callbacks, nor does it support custom datatypes on
# callbacks, so we must ensure that all callbacks return
# primitive datatypes.
#
# Non-primitive return values wrapped with UNCHECKED won't be
# typechecked, and will be converted to c_void_p.
def UNCHECKED(type):
    if (hasattr(type, "_type_") and isinstance(type._type_, str)
        and type._type_ != "P"):
        return type
    else:
        return c_void_p

# ctypes doesn't have direct support for variadic functions, so we have to write
# our own wrapper class
class _variadic_function(object):
    def __init__(self,func,restype,argtypes):
        self.func=func
        self.func.restype=restype
        self.argtypes=argtypes
    def _as_parameter_(self):
        # So we can pass this variadic function as a function pointer
        return self.func
    def __call__(self,*args):
        fixed_args=[]
        i=0
        for argtype in self.argtypes:
            # Typecheck what we can
            fixed_args.append(argtype.from_param(args[i]))
            i+=1
        return self.func(*fixed_args+list(args[i:]))

# End preamble

_libs = {}
_libdirs = []

# Begin loader

# ----------------------------------------------------------------------------
# Copyright (c) 2008 David James
# Copyright (c) 2006-2008 Alex Holkner
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
#  * Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
#  * Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in
#    the documentation and/or other materials provided with the
#    distribution.
#  * Neither the name of pyglet nor the names of its
#    contributors may be used to endorse or promote products
#    derived from this software without specific prior written
#    permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
# FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
# COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
# BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
# ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
# ----------------------------------------------------------------------------

import os.path, re, sys, glob
import platform
import ctypes
import ctypes.util

def _environ_path(name):
    if name in os.environ:
        return os.environ[name].split(":")
    else:
        return []

class LibraryLoader(object):
    def __init__(self):
        self.other_dirs=[]

    def load_library(self,libname):
        """Given the name of a library, load it."""
        paths = self.getpaths(libname)

        for path in paths:
            if os.path.exists(path):
                return self.load(path)

        raise ImportError("%s not found." % libname)

    def load(self,path):
        """Given a path to a library, load it."""
        try:
            # Darwin requires dlopen to be called with mode RTLD_GLOBAL instead
            # of the default RTLD_LOCAL.  Without this, you end up with
            # libraries not being loadable, resulting in "Symbol not found"
            # errors
            if sys.platform == 'darwin':
                return ctypes.CDLL(path, ctypes.RTLD_GLOBAL)
            else:
                return ctypes.cdll.LoadLibrary(path)
        except OSError,e:
            raise ImportError(e)

    def getpaths(self,libname):
        """Return a list of paths where the library might be found."""
        if os.path.isabs(libname):
            yield libname
        else:
            # FIXME / TODO return '.' and os.path.dirname(__file__)
            for path in self.getplatformpaths(libname):
                yield path

            path = ctypes.util.find_library(libname)
            if path: yield path

    def getplatformpaths(self, libname):
        return []

# Darwin (Mac OS X)

class DarwinLibraryLoader(LibraryLoader):
    name_formats = ["lib%s.dylib", "lib%s.so", "lib%s.bundle", "%s.dylib",
                "%s.so", "%s.bundle", "%s"]

    def getplatformpaths(self,libname):
        if os.path.pathsep in libname:
            names = [libname]
        else:
            names = [format % libname for format in self.name_formats]

        for dir in self.getdirs(libname):
            for name in names:
                yield os.path.join(dir,name)

    def getdirs(self,libname):
        '''Implements the dylib search as specified in Apple documentation:

        http://developer.apple.com/documentation/DeveloperTools/Conceptual/
            DynamicLibraries/Articles/DynamicLibraryUsageGuidelines.html

        Before commencing the standard search, the method first checks
        the bundle's ``Frameworks`` directory if the application is running
        within a bundle (OS X .app).
        '''

        dyld_fallback_library_path = _environ_path("DYLD_FALLBACK_LIBRARY_PATH")
        if not dyld_fallback_library_path:
            dyld_fallback_library_path = [os.path.expanduser('~/lib'),
                                          '/usr/local/lib', '/usr/lib']

        dirs = []

        if '/' in libname:
            dirs.extend(_environ_path("DYLD_LIBRARY_PATH"))
        else:
            dirs.extend(_environ_path("LD_LIBRARY_PATH"))
            dirs.extend(_environ_path("DYLD_LIBRARY_PATH"))

        dirs.extend(self.other_dirs)
        dirs.append(".")
        dirs.append(os.path.dirname(__file__))

        if hasattr(sys, 'frozen') and sys.frozen == 'macosx_app':
            dirs.append(os.path.join(
                os.environ['RESOURCEPATH'],
                '..',
                'Frameworks'))

        dirs.extend(dyld_fallback_library_path)

        return dirs

# Posix

class PosixLibraryLoader(LibraryLoader):
    _ld_so_cache = None

    def _create_ld_so_cache(self):
        # Recreate search path followed by ld.so.  This is going to be
        # slow to build, and incorrect (ld.so uses ld.so.cache, which may
        # not be up-to-date).  Used only as fallback for distros without
        # /sbin/ldconfig.
        #
        # We assume the DT_RPATH and DT_RUNPATH binary sections are omitted.

        directories = []
        for name in ("LD_LIBRARY_PATH",
                     "SHLIB_PATH", # HPUX
                     "LIBPATH", # OS/2, AIX
                     "LIBRARY_PATH", # BE/OS
                    ):
            if name in os.environ:
                directories.extend(os.environ[name].split(os.pathsep))
        directories.extend(self.other_dirs)
        directories.append(".")
        directories.append(os.path.dirname(__file__))

        try: directories.extend([dir.strip() for dir in open('/etc/ld.so.conf')])
        except IOError: pass

        unix_lib_dirs_list = ['/lib', '/usr/lib', '/lib64', '/usr/lib64']
        if sys.platform.startswith('linux'):
            # Try and support multiarch work in Ubuntu
            # https://wiki.ubuntu.com/MultiarchSpec
            bitage = platform.architecture()[0]
            if bitage.startswith('32'):
                # Assume Intel/AMD x86 compat
                unix_lib_dirs_list += ['/lib/i386-linux-gnu', '/usr/lib/i386-linux-gnu']
            elif bitage.startswith('64'):
                # Assume Intel/AMD x86 compat
                unix_lib_dirs_list += ['/lib/x86_64-linux-gnu', '/usr/lib/x86_64-linux-gnu']
            else:
                # guess...
                unix_lib_dirs_list += glob.glob('/lib/*linux-gnu')
        directories.extend(unix_lib_dirs_list)

        cache = {}
        lib_re = re.compile(r'lib(.*)\.s[ol]')
        ext_re = re.compile(r'\.s[ol]$')
        for dir in directories:
            try:
                for path in glob.glob("%s/*.s[ol]*" % dir):
                    file = os.path.basename(path)

                    # Index by filename
                    if file not in cache:
                        cache[file] = path

                    # Index by library name
                    match = lib_re.match(file)
                    if match:
                        library = match.group(1)
                        if library not in cache:
                            cache[library] = path
            except OSError:
                pass

        self._ld_so_cache = cache

    def getplatformpaths(self, libname):
        if self._ld_so_cache is None:
            self._create_ld_so_cache()

        result = self._ld_so_cache.get(libname)
        if result: yield result

        path = ctypes.util.find_library(libname)
        if path: yield os.path.join("/lib",path)

# Windows

class _WindowsLibrary(object):
    def __init__(self, path):
        self.cdll = ctypes.cdll.LoadLibrary(path)
        self.windll = ctypes.windll.LoadLibrary(path)

    def __getattr__(self, name):
        try: return getattr(self.cdll,name)
        except AttributeError:
            try: return getattr(self.windll,name)
            except AttributeError:
                raise

class WindowsLibraryLoader(LibraryLoader):
    name_formats = ["%s.dll", "lib%s.dll", "%slib.dll"]

    def load_library(self, libname):
        try:
            result = LibraryLoader.load_library(self, libname)
        except ImportError:
            result = None
            if os.path.sep not in libname:
                for name in self.name_formats:
                    try:
                        result = getattr(ctypes.cdll, name % libname)
                        if result:
                            break
                    except WindowsError:
                        result = None
            if result is None:
                try:
                    result = getattr(ctypes.cdll, libname)
                except WindowsError:
                    result = None
            if result is None:
                raise ImportError("%s not found." % libname)
        return result

    def load(self, path):
        return _WindowsLibrary(path)

    def getplatformpaths(self, libname):
        if os.path.sep not in libname:
            for name in self.name_formats:
                dll_in_current_dir = os.path.abspath(name % libname)
                if os.path.exists(dll_in_current_dir):
                    yield dll_in_current_dir
                path = ctypes.util.find_library(name % libname)
                if path:
                    yield path

# Platform switching

# If your value of sys.platform does not appear in this dict, please contact
# the Ctypesgen maintainers.

loaderclass = {
    "darwin":   DarwinLibraryLoader,
    "cygwin":   WindowsLibraryLoader,
    "win32":    WindowsLibraryLoader
}

loader = loaderclass.get(sys.platform, PosixLibraryLoader)()

def add_library_search_dirs(other_dirs):
    loader.other_dirs = other_dirs

load_library = loader.load_library

del loaderclass

# End loader

add_library_search_dirs([])

# No libraries

# No modules

# test/tc/times_two.h: 11
class struct_times_two_tc_params(Structure):
    pass

struct_times_two_tc_params.__slots__ = [
    'input',
]
struct_times_two_tc_params._fields_ = [
    ('input', c_int),
]

# test/tc/times_two.h: 11
for _lib in _libs.values():
    try:
        times_two_tc_params = (struct_times_two_tc_params).in_dll(_lib, 'times_two_tc_params')
        break
    except:
        pass

# test/tc/times_two.h: 15
class struct_times_two_tc_results(Structure):
    pass

struct_times_two_tc_results.__slots__ = [
    'result',
]
struct_times_two_tc_results._fields_ = [
    ('result', c_int),
]

# test/tc/times_two.h: 15
for _lib in _libs.values():
    try:
        times_two_tc_results = (struct_times_two_tc_results).in_dll(_lib, 'times_two_tc_results')
        break
    except:
        pass

# test/tc/osc8m_calib.h: 13
class struct_osc8m_calib_tc_params(Structure):
    pass

struct_osc8m_calib_tc_params.__slots__ = [
    'dummy',
]
struct_osc8m_calib_tc_params._fields_ = [
    ('dummy', c_int),
]

# test/tc/osc8m_calib.h: 13
for _lib in _libs.values():
    try:
        osc8m_calib_tc_params = (struct_osc8m_calib_tc_params).in_dll(_lib, 'osc8m_calib_tc_params')
        break
    except:
        pass

# test/tc/osc8m_calib.h: 19
class struct_osc8m_calib_tc_results(Structure):
    pass

struct_osc8m_calib_tc_results.__slots__ = [
    'result',
    'c_process',
    'c_temp',
]
struct_osc8m_calib_tc_results._fields_ = [
    ('result', c_int),
    ('c_process', c_int),
    ('c_temp', c_int),
]

# test/tc/osc8m_calib.h: 19
for _lib in _libs.values():
    try:
        osc8m_calib_tc_results = (struct_osc8m_calib_tc_results).in_dll(_lib, 'osc8m_calib_tc_results')
        break
    except:
        pass

# test/tc/osc8m_calib.h: 22
for _lib in _libs.values():
    try:
        _result = (c_uint32).in_dll(_lib, '_result')
        break
    except:
        pass

# inc/cron.h: 33
class struct_tracker_time(Structure):
    pass

struct_tracker_time.__slots__ = [
    'epoch',
    'year',
    'month',
    'day',
    'hour',
    'minute',
    'second',
    'valid',
]
struct_tracker_time._fields_ = [
    ('epoch', c_uint32),
    ('year', c_uint16),
    ('month', c_uint8),
    ('day', c_uint8),
    ('hour', c_uint8),
    ('minute', c_uint8),
    ('second', c_uint8),
    ('valid', c_uint8),
]

# inc/data.h: 59
class struct_tracker_datapoint(Structure):
    pass

struct_tracker_datapoint.__slots__ = [
    'time',
    'latitude',
    'longitude',
    'altitude',
    'satillite_count',
    'time_to_first_fix',
    'battery',
    'solar',
    'main_pressure',
    'thermistor_temperature',
    'bmp180_temperature',
    'radio_die_temperature',
    'xosc_error',
    'flash_status',
]
struct_tracker_datapoint._fields_ = [
    ('time', struct_tracker_time),
    ('latitude', c_int32),
    ('longitude', c_int32),
    ('altitude', c_int32),
    ('satillite_count', c_uint8),
    ('time_to_first_fix', c_uint8),
    ('battery', c_float),
    ('solar', c_float),
    ('main_pressure', c_int32),
    ('thermistor_temperature', c_float),
    ('bmp180_temperature', c_float),
    ('radio_die_temperature', c_float),
    ('xosc_error', c_uint32),
    ('flash_status', c_uint8),
]

# test/tc/location_aprs.h: 16
class struct_location_aprs_tc_params(Structure):
    pass

struct_location_aprs_tc_params.__slots__ = [
    'lat',
    'lon',
]
struct_location_aprs_tc_params._fields_ = [
    ('lat', c_float),
    ('lon', c_float),
]

# test/tc/location_aprs.h: 16
for _lib in _libs.values():
    try:
        location_aprs_tc_params = (struct_location_aprs_tc_params).in_dll(_lib, 'location_aprs_tc_params')
        break
    except:
        pass

# test/tc/location_aprs.h: 23
class struct_location_aprs_tc_results(Structure):
    pass

struct_location_aprs_tc_results.__slots__ = [
    'tx_allow',
    'frequency',
    'prefix',
    'callsign',
]
struct_location_aprs_tc_results._fields_ = [
    ('tx_allow', c_bool),
    ('frequency', c_double),
    ('prefix', String),
    ('callsign', String),
]

# test/tc/location_aprs.h: 23
for _lib in _libs.values():
    try:
        location_aprs_tc_results = (struct_location_aprs_tc_results).in_dll(_lib, 'location_aprs_tc_results')
        break
    except:
        pass

# test/tc/location_telemetry.h: 15
class struct_location_telemetry_tc_params(Structure):
    pass

struct_location_telemetry_tc_params.__slots__ = [
    'lat',
    'lon',
]
struct_location_telemetry_tc_params._fields_ = [
    ('lat', c_float),
    ('lon', c_float),
]

# test/tc/location_telemetry.h: 15
for _lib in _libs.values():
    try:
        location_telemetry_tc_params = (struct_location_telemetry_tc_params).in_dll(_lib, 'location_telemetry_tc_params')
        break
    except:
        pass

# test/tc/location_telemetry.h: 19
class struct_location_telemetry_tc_results(Structure):
    pass

struct_location_telemetry_tc_results.__slots__ = [
    'tx_allow',
]
struct_location_telemetry_tc_results._fields_ = [
    ('tx_allow', c_bool),
]

# test/tc/location_telemetry.h: 19
for _lib in _libs.values():
    try:
        location_telemetry_tc_results = (struct_location_telemetry_tc_results).in_dll(_lib, 'location_telemetry_tc_results')
        break
    except:
        pass

# test/tc/mem_write_page.h: 18
class struct_mem_write_page_tc_params(Structure):
    pass

struct_mem_write_page_tc_params.__slots__ = [
    'address',
    'page',
]
struct_mem_write_page_tc_params._fields_ = [
    ('address', c_uint32),
    ('page', c_uint8 * 256),
]

# test/tc/mem_write_page.h: 18
for _lib in _libs.values():
    try:
        mem_write_page_tc_params = (struct_mem_write_page_tc_params).in_dll(_lib, 'mem_write_page_tc_params')
        break
    except:
        pass

# test/tc/mem_write_page.h: 25
class struct_mem_write_page_tc_results(Structure):
    pass

struct_mem_write_page_tc_results.__slots__ = [
    'page_read',
]
struct_mem_write_page_tc_results._fields_ = [
    ('page_read', c_uint8 * 256),
]

# test/tc/mem_write_page.h: 25
for _lib in _libs.values():
    try:
        mem_write_page_tc_results = (struct_mem_write_page_tc_results).in_dll(_lib, 'mem_write_page_tc_results')
        break
    except:
        pass

# test/tc/mem_write_all.h: 17
class struct_mem_write_all_tc_params(Structure):
    pass

struct_mem_write_all_tc_params.__slots__ = [
    'page_data',
]
struct_mem_write_all_tc_params._fields_ = [
    ('page_data', c_uint8 * 256),
]

# test/tc/mem_write_all.h: 17
for _lib in _libs.values():
    try:
        mem_write_all_tc_params = (struct_mem_write_all_tc_params).in_dll(_lib, 'mem_write_all_tc_params')
        break
    except:
        pass

# test/tc/mem_write_all.h: 26
class struct_mem_write_all_tc_results(Structure):
    pass

struct_mem_write_all_tc_results.__slots__ = [
    'all_good',
    'fail_address',
    'fail_wrote',
    'fail_read',
]
struct_mem_write_all_tc_results._fields_ = [
    ('all_good', c_uint8),
    ('fail_address', c_uint32),
    ('fail_wrote', c_uint8),
    ('fail_read', c_uint8),
]

# test/tc/mem_write_all.h: 26
for _lib in _libs.values():
    try:
        mem_write_all_tc_results = (struct_mem_write_all_tc_results).in_dll(_lib, 'mem_write_all_tc_results')
        break
    except:
        pass

# test/tc/mem_write_all.h: 36
for _lib in _libs.values():
    try:
        page_read = (c_uint8 * 256).in_dll(_lib, 'page_read')
        break
    except:
        pass

# test/tc/mem_write_all.h: 37
for _lib in _libs.values():
    try:
        i = (c_uint32).in_dll(_lib, 'i')
        break
    except:
        pass

# test/tc/mem_write_all.h: 37
for _lib in _libs.values():
    try:
        j = (c_uint32).in_dll(_lib, 'j')
        break
    except:
        pass

# test/tc/location_aprs_file.h: 17
class struct_location_aprs_file_tc_params(Structure):
    pass

struct_location_aprs_file_tc_params.__slots__ = [
    'lat',
    'lon',
]
struct_location_aprs_file_tc_params._fields_ = [
    ('lat', c_float),
    ('lon', c_float),
]

# test/tc/location_aprs_file.h: 17
for _lib in _libs.values():
    try:
        location_aprs_file_tc_params = (struct_location_aprs_file_tc_params).in_dll(_lib, 'location_aprs_file_tc_params')
        break
    except:
        pass

# test/tc/location_aprs_file.h: 25
class struct_location_aprs_file_tc_results(Structure):
    pass

struct_location_aprs_file_tc_results.__slots__ = [
    'tx_allow',
    'frequency',
]
struct_location_aprs_file_tc_results._fields_ = [
    ('tx_allow', c_bool),
    ('frequency', c_float),
]

# test/tc/location_aprs_file.h: 25
for _lib in _libs.values():
    try:
        location_aprs_file_tc_results = (struct_location_aprs_file_tc_results).in_dll(_lib, 'location_aprs_file_tc_results')
        break
    except:
        pass

# test/tc/backlog_write_read.h: 19
class struct_backlog_write_read_tc_params(Structure):
    pass

struct_backlog_write_read_tc_params.__slots__ = [
    'record_not_get',
    'epoch_write',
]
struct_backlog_write_read_tc_params._fields_ = [
    ('record_not_get', c_uint8),
    ('epoch_write', c_uint32),
]

# test/tc/backlog_write_read.h: 19
for _lib in _libs.values():
    try:
        backlog_write_read_tc_params = (struct_backlog_write_read_tc_params).in_dll(_lib, 'backlog_write_read_tc_params')
        break
    except:
        pass

# test/tc/backlog_write_read.h: 27
class struct_backlog_write_read_tc_results(Structure):
    pass

struct_backlog_write_read_tc_results.__slots__ = [
    'epoch_read',
    'wptr',
    'rptr',
]
struct_backlog_write_read_tc_results._fields_ = [
    ('epoch_read', c_uint32),
    ('wptr', c_uint32),
    ('rptr', c_uint32),
]

# test/tc/backlog_write_read.h: 27
for _lib in _libs.values():
    try:
        backlog_write_read_tc_results = (struct_backlog_write_read_tc_results).in_dll(_lib, 'backlog_write_read_tc_results')
        break
    except:
        pass

# test/tc/backlog_write_read.h: 29
for _lib in _libs.values():
    try:
        backlog_write_index = (c_uint16).in_dll(_lib, 'backlog_write_index')
        break
    except:
        pass

# test/tc/backlog_write_read.h: 32
for _lib in _libs.values():
    try:
        dp = (struct_tracker_datapoint).in_dll(_lib, 'dp')
        break
    except:
        pass

# test/tc/backlog_write_read.h: 33
for _lib in _libs.values():
    try:
        dp_ptr = (POINTER(struct_tracker_datapoint)).in_dll(_lib, 'dp_ptr')
        break
    except:
        pass

# test/tc/backlog_read.h: 20
class struct_backlog_read_tc_params(Structure):
    pass

struct_backlog_read_tc_params.__slots__ = [
    'dummy',
]
struct_backlog_read_tc_params._fields_ = [
    ('dummy', c_uint32),
]

# test/tc/backlog_read.h: 20
for _lib in _libs.values():
    try:
        backlog_read_tc_params = (struct_backlog_read_tc_params).in_dll(_lib, 'backlog_read_tc_params')
        break
    except:
        pass

# test/tc/backlog_read.h: 28
class struct_backlog_read_tc_results(Structure):
    pass

struct_backlog_read_tc_results.__slots__ = [
    'aprs_backlog_str',
    'returned_null',
]
struct_backlog_read_tc_results._fields_ = [
    ('aprs_backlog_str', c_char * 256),
    ('returned_null', c_uint8),
]

# test/tc/backlog_read.h: 28
for _lib in _libs.values():
    try:
        backlog_read_tc_results = (struct_backlog_read_tc_results).in_dll(_lib, 'backlog_read_tc_results')
        break
    except:
        pass

# test/tc/backlog_read.h: 30
for _lib in _libs.values():
    try:
        backlog_index = (c_uint16).in_dll(_lib, 'backlog_index')
        break
    except:
        pass

# test/tc/backlog_read.h: 33
for _lib in _libs.values():
    try:
        dp_ptr = (POINTER(struct_tracker_datapoint)).in_dll(_lib, 'dp_ptr')
        break
    except:
        pass

# test/tc/mem_erase_all.h: 17
class struct_mem_erase_all_tc_params(Structure):
    pass

struct_mem_erase_all_tc_params.__slots__ = [
    'dummy',
]
struct_mem_erase_all_tc_params._fields_ = [
    ('dummy', c_uint32),
]

# test/tc/mem_erase_all.h: 17
for _lib in _libs.values():
    try:
        mem_erase_all_tc_params = (struct_mem_erase_all_tc_params).in_dll(_lib, 'mem_erase_all_tc_params')
        break
    except:
        pass

# test/tc/mem_erase_all.h: 24
class struct_mem_erase_all_tc_results(Structure):
    pass

struct_mem_erase_all_tc_results.__slots__ = [
    'dummy',
]
struct_mem_erase_all_tc_results._fields_ = [
    ('dummy', c_uint32),
]

# test/tc/mem_erase_all.h: 24
for _lib in _libs.values():
    try:
        mem_erase_all_tc_results = (struct_mem_erase_all_tc_results).in_dll(_lib, 'mem_erase_all_tc_results')
        break
    except:
        pass

# test/tc/analogue_read.h: 18
class struct_analogue_read_tc_params(Structure):
    pass

struct_analogue_read_tc_params.__slots__ = [
    'dummy',
]
struct_analogue_read_tc_params._fields_ = [
    ('dummy', c_uint32),
]

# test/tc/analogue_read.h: 18
for _lib in _libs.values():
    try:
        analogue_read_tc_params = (struct_analogue_read_tc_params).in_dll(_lib, 'analogue_read_tc_params')
        break
    except:
        pass

# test/tc/analogue_read.h: 27
class struct_analogue_read_tc_results(Structure):
    pass

struct_analogue_read_tc_results.__slots__ = [
    'battery',
    'thermistor',
    'solar',
]
struct_analogue_read_tc_results._fields_ = [
    ('battery', c_float),
    ('thermistor', c_float),
    ('solar', c_float),
]

# test/tc/analogue_read.h: 27
for _lib in _libs.values():
    try:
        analogue_read_tc_results = (struct_analogue_read_tc_results).in_dll(_lib, 'analogue_read_tc_results')
        break
    except:
        pass

# test/tc/adc_battery_solar_read.h: 17
class struct_adc_battery_solar_read_tc_params(Structure):
    pass

struct_adc_battery_solar_read_tc_params.__slots__ = [
    'dummy',
]
struct_adc_battery_solar_read_tc_params._fields_ = [
    ('dummy', c_uint32),
]

# test/tc/adc_battery_solar_read.h: 17
for _lib in _libs.values():
    try:
        adc_battery_solar_read_tc_params = (struct_adc_battery_solar_read_tc_params).in_dll(_lib, 'adc_battery_solar_read_tc_params')
        break
    except:
        pass

# test/tc/adc_battery_solar_read.h: 25
class struct_adc_battery_solar_read_tc_results(Structure):
    pass

struct_adc_battery_solar_read_tc_results.__slots__ = [
    'battery',
    'solar',
]
struct_adc_battery_solar_read_tc_results._fields_ = [
    ('battery', c_float),
    ('solar', c_float),
]

# test/tc/adc_battery_solar_read.h: 25
for _lib in _libs.values():
    try:
        adc_battery_solar_read_tc_results = (struct_adc_battery_solar_read_tc_results).in_dll(_lib, 'adc_battery_solar_read_tc_results')
        break
    except:
        pass

# inc/gps.h: 59
class struct_gps_data_t(Structure):
    pass

struct_gps_data_t.__slots__ = [
    'year',
    'month',
    'day',
    'hour',
    'minute',
    'second',
    'latitude',
    'longitude',
    'altitude',
    'satillite_count',
    'is_locked',
    'time_to_first_fix',
]
struct_gps_data_t._fields_ = [
    ('year', c_uint16),
    ('month', c_uint8),
    ('day', c_uint8),
    ('hour', c_uint8),
    ('minute', c_uint8),
    ('second', c_uint8),
    ('latitude', c_int32),
    ('longitude', c_int32),
    ('altitude', c_int32),
    ('satillite_count', c_uint8),
    ('is_locked', c_uint8),
    ('time_to_first_fix', c_uint8),
]

# test/tc/gps_baud_error.h: 20
class struct_gps_baud_error_tc_params(Structure):
    pass

struct_gps_baud_error_tc_params.__slots__ = [
    'dummy',
]
struct_gps_baud_error_tc_params._fields_ = [
    ('dummy', c_uint32),
]

# test/tc/gps_baud_error.h: 20
for _lib in _libs.values():
    try:
        gps_baud_error_tc_params = (struct_gps_baud_error_tc_params).in_dll(_lib, 'gps_baud_error_tc_params')
        break
    except:
        pass

# test/tc/gps_baud_error.h: 30
class struct_gps_baud_error_tc_results(Structure):
    pass

struct_gps_baud_error_tc_results.__slots__ = [
    'intended_baud',
    'peripheral_clock',
    'register_value',
    'calculated_baud_milli',
]
struct_gps_baud_error_tc_results._fields_ = [
    ('intended_baud', c_uint32),
    ('peripheral_clock', c_uint32),
    ('register_value', c_uint16),
    ('calculated_baud_milli', c_uint32),
]

# test/tc/gps_baud_error.h: 30
for _lib in _libs.values():
    try:
        gps_baud_error_tc_results = (struct_gps_baud_error_tc_results).in_dll(_lib, 'gps_baud_error_tc_results')
        break
    except:
        pass

# test/tc/gps_baud_error.h: 41
for _lib in _libs.values():
    try:
        sercom_index = (c_uint32).in_dll(_lib, 'sercom_index')
        break
    except:
        pass

# test/tc/gps_baud_error.h: 42
for _lib in _libs.values():
    try:
        gclk_index = (c_uint32).in_dll(_lib, 'gclk_index')
        break
    except:
        pass

# test/tc/gps_baud_error.h: 43
for _lib in _libs.values():
    try:
        baudrate = (c_uint32).in_dll(_lib, 'baudrate')
        break
    except:
        pass

# test/tc/gps_baud_error.h: 44
for _lib in _libs.values():
    try:
        baud = (c_uint16).in_dll(_lib, 'baud')
        break
    except:
        pass

# test/tc/gps_baud_error.h: 45
for _lib in _libs.values():
    try:
        calcuated_baud = (c_uint32).in_dll(_lib, 'calcuated_baud')
        break
    except:
        pass

# test/tc/crc32_gen_buf.h: 19
class struct_crc32_gen_buf_tc_params(Structure):
    pass

struct_crc32_gen_buf_tc_params.__slots__ = [
    'test_buffer',
]
struct_crc32_gen_buf_tc_params._fields_ = [
    ('test_buffer', c_uint8 * 32),
]

# test/tc/crc32_gen_buf.h: 19
for _lib in _libs.values():
    try:
        crc32_gen_buf_tc_params = (struct_crc32_gen_buf_tc_params).in_dll(_lib, 'crc32_gen_buf_tc_params')
        break
    except:
        pass

# test/tc/crc32_gen_buf.h: 27
class struct_crc32_gen_buf_tc_results(Structure):
    pass

struct_crc32_gen_buf_tc_results.__slots__ = [
    'calculated_crc32',
    'extracted_crc32',
]
struct_crc32_gen_buf_tc_results._fields_ = [
    ('calculated_crc32', c_uint32),
    ('extracted_crc32', c_uint32),
]

# test/tc/crc32_gen_buf.h: 27
for _lib in _libs.values():
    try:
        crc32_gen_buf_tc_results = (struct_crc32_gen_buf_tc_results).in_dll(_lib, 'crc32_gen_buf_tc_results')
        break
    except:
        pass

# test/tc/crc32_gen_buf.h: 37
for _lib in _libs.values():
    try:
        test = (c_uint32).in_dll(_lib, 'test')
        break
    except:
        pass

# test/tc/crc32_gen_buf.h: 38
for _lib in _libs.values():
    try:
        uint8t_buffer = (POINTER(c_uint8)).in_dll(_lib, 'uint8t_buffer')
        break
    except:
        pass

# test/tc/barometric_altitude.h: 17
class struct_barometric_altitude_tc_params(Structure):
    pass

struct_barometric_altitude_tc_params.__slots__ = [
    'pressure',
]
struct_barometric_altitude_tc_params._fields_ = [
    ('pressure', c_float),
]

# test/tc/barometric_altitude.h: 17
for _lib in _libs.values():
    try:
        barometric_altitude_tc_params = (struct_barometric_altitude_tc_params).in_dll(_lib, 'barometric_altitude_tc_params')
        break
    except:
        pass

# test/tc/barometric_altitude.h: 24
class struct_barometric_altitude_tc_results(Structure):
    pass

struct_barometric_altitude_tc_results.__slots__ = [
    'altitude',
]
struct_barometric_altitude_tc_results._fields_ = [
    ('altitude', c_double),
]

# test/tc/barometric_altitude.h: 24
for _lib in _libs.values():
    try:
        barometric_altitude_tc_results = (struct_barometric_altitude_tc_results).in_dll(_lib, 'barometric_altitude_tc_results')
        break
    except:
        pass

# test/tc/pressure_temperature.h: 19
class struct_pressure_temperature_tc_params(Structure):
    pass

struct_pressure_temperature_tc_params.__slots__ = [
    'dummy',
]
struct_pressure_temperature_tc_params._fields_ = [
    ('dummy', c_uint32),
]

# test/tc/pressure_temperature.h: 19
for _lib in _libs.values():
    try:
        pressure_temperature_tc_params = (struct_pressure_temperature_tc_params).in_dll(_lib, 'pressure_temperature_tc_params')
        break
    except:
        pass

# test/tc/pressure_temperature.h: 26
class struct_pressure_temperature_tc_results(Structure):
    pass

struct_pressure_temperature_tc_results.__slots__ = [
    'pressure',
    'temperature',
]
struct_pressure_temperature_tc_results._fields_ = [
    ('pressure', c_float),
    ('temperature', c_float),
]

# test/tc/pressure_temperature.h: 26
for _lib in _libs.values():
    try:
        pressure_temperature_tc_results = (struct_pressure_temperature_tc_results).in_dll(_lib, 'pressure_temperature_tc_results')
        break
    except:
        pass

# test/tc/thermistor_equation.h: 18
class struct_thermistor_equation_tc_params(Structure):
    pass

struct_thermistor_equation_tc_params.__slots__ = [
    'value',
]
struct_thermistor_equation_tc_params._fields_ = [
    ('value', c_float),
]

# test/tc/thermistor_equation.h: 18
for _lib in _libs.values():
    try:
        thermistor_equation_tc_params = (struct_thermistor_equation_tc_params).in_dll(_lib, 'thermistor_equation_tc_params')
        break
    except:
        pass

# test/tc/thermistor_equation.h: 25
class struct_thermistor_equation_tc_results(Structure):
    pass

struct_thermistor_equation_tc_results.__slots__ = [
    'temperature',
]
struct_thermistor_equation_tc_results._fields_ = [
    ('temperature', c_float),
]

# test/tc/thermistor_equation.h: 25
for _lib in _libs.values():
    try:
        thermistor_equation_tc_results = (struct_thermistor_equation_tc_results).in_dll(_lib, 'thermistor_equation_tc_results')
        break
    except:
        pass

# test/tc/gps_poll.h: 17
class struct_gps_poll_tc_params(Structure):
    pass

struct_gps_poll_tc_params.__slots__ = [
    'dummy',
]
struct_gps_poll_tc_params._fields_ = [
    ('dummy', c_uint32),
]

# test/tc/gps_poll.h: 17
for _lib in _libs.values():
    try:
        gps_poll_tc_params = (struct_gps_poll_tc_params).in_dll(_lib, 'gps_poll_tc_params')
        break
    except:
        pass

# test/tc/gps_poll.h: 27
class struct_gps_poll_tc_results(Structure):
    pass

struct_gps_poll_tc_results.__slots__ = [
    'latitude',
    'longitude',
    'altitude',
    'satillite_count',
    'is_locked',
]
struct_gps_poll_tc_results._fields_ = [
    ('latitude', c_int32),
    ('longitude', c_int32),
    ('altitude', c_int32),
    ('satillite_count', c_uint8),
    ('is_locked', c_uint8),
]

# test/tc/gps_poll.h: 27
for _lib in _libs.values():
    try:
        gps_poll_tc_results = (struct_gps_poll_tc_results).in_dll(_lib, 'gps_poll_tc_results')
        break
    except:
        pass

# test/tc/gps_poll.h: 38
for _lib in _libs.values():
    try:
        data = (struct_gps_data_t).in_dll(_lib, 'data')
        break
    except:
        pass

# test/tc/epoch_from_time.h: 17
class struct_epoch_from_time_tc_params(Structure):
    pass

struct_epoch_from_time_tc_params.__slots__ = [
    'dummy',
]
struct_epoch_from_time_tc_params._fields_ = [
    ('dummy', c_int),
]

# test/tc/epoch_from_time.h: 17
for _lib in _libs.values():
    try:
        epoch_from_time_tc_params = (struct_epoch_from_time_tc_params).in_dll(_lib, 'epoch_from_time_tc_params')
        break
    except:
        pass

# test/tc/epoch_from_time.h: 24
class struct_epoch_from_time_tc_results(Structure):
    pass

struct_epoch_from_time_tc_results.__slots__ = [
    'epoch',
]
struct_epoch_from_time_tc_results._fields_ = [
    ('epoch', c_uint32),
]

# test/tc/epoch_from_time.h: 24
for _lib in _libs.values():
    try:
        epoch_from_time_tc_results = (struct_epoch_from_time_tc_results).in_dll(_lib, 'epoch_from_time_tc_results')
        break
    except:
        pass

# test/tc/epoch_from_time.h: 35
for _lib in _libs.values():
    try:
        t = (struct_tracker_time).in_dll(_lib, 't')
        break
    except:
        pass

tc_ptr_type = CFUNCTYPE(UNCHECKED(None), ) # /richard/_pico-tracker/firmware/test/tmain.c: 65

# /richard/_pico-tracker/firmware/test/tmain.c: 66
for _lib in _libs.values():
    try:
        tc_ptr = (tc_ptr_type).in_dll(_lib, 'tc_ptr')
        break
    except:
        pass

# /richard/_pico-tracker/firmware/test/tc/osc8m_calib.h: 13
for _lib in _libs.values():
    try:
        osc8m_calib_tc_params = (struct_osc8m_calib_tc_params).in_dll(_lib, 'osc8m_calib_tc_params')
        break
    except:
        pass

# /richard/_pico-tracker/firmware/test/tc/osc8m_calib.h: 19
for _lib in _libs.values():
    try:
        osc8m_calib_tc_results = (struct_osc8m_calib_tc_results).in_dll(_lib, 'osc8m_calib_tc_results')
        break
    except:
        pass

# /richard/_pico-tracker/firmware/test/tc/osc8m_calib.h: 22
for _lib in _libs.values():
    try:
        _result = (c_uint32).in_dll(_lib, '_result')
        break
    except:
        pass

# /richard/_pico-tracker/firmware/test/tc/mem_write_page.h: 18
for _lib in _libs.values():
    try:
        mem_write_page_tc_params = (struct_mem_write_page_tc_params).in_dll(_lib, 'mem_write_page_tc_params')
        break
    except:
        pass

# /richard/_pico-tracker/firmware/test/tc/mem_write_page.h: 25
for _lib in _libs.values():
    try:
        mem_write_page_tc_results = (struct_mem_write_page_tc_results).in_dll(_lib, 'mem_write_page_tc_results')
        break
    except:
        pass

# /richard/_pico-tracker/firmware/test/tc/location_aprs_file.h: 17
for _lib in _libs.values():
    try:
        location_aprs_file_tc_params = (struct_location_aprs_file_tc_params).in_dll(_lib, 'location_aprs_file_tc_params')
        break
    except:
        pass

# /richard/_pico-tracker/firmware/test/tc/location_aprs_file.h: 25
for _lib in _libs.values():
    try:
        location_aprs_file_tc_results = (struct_location_aprs_file_tc_results).in_dll(_lib, 'location_aprs_file_tc_results')
        break
    except:
        pass

# /richard/_pico-tracker/firmware/test/tc/barometric_altitude.h: 17
for _lib in _libs.values():
    try:
        barometric_altitude_tc_params = (struct_barometric_altitude_tc_params).in_dll(_lib, 'barometric_altitude_tc_params')
        break
    except:
        pass

# /richard/_pico-tracker/firmware/test/tc/barometric_altitude.h: 24
for _lib in _libs.values():
    try:
        barometric_altitude_tc_results = (struct_barometric_altitude_tc_results).in_dll(_lib, 'barometric_altitude_tc_results')
        break
    except:
        pass

# /richard/_pico-tracker/firmware/test/tc/backlog_write_read.h: 19
for _lib in _libs.values():
    try:
        backlog_write_read_tc_params = (struct_backlog_write_read_tc_params).in_dll(_lib, 'backlog_write_read_tc_params')
        break
    except:
        pass

# /richard/_pico-tracker/firmware/test/tc/backlog_write_read.h: 27
for _lib in _libs.values():
    try:
        backlog_write_read_tc_results = (struct_backlog_write_read_tc_results).in_dll(_lib, 'backlog_write_read_tc_results')
        break
    except:
        pass

# /richard/_pico-tracker/firmware/test/tc/backlog_write_read.h: 29
for _lib in _libs.values():
    try:
        backlog_write_index = (c_uint16).in_dll(_lib, 'backlog_write_index')
        break
    except:
        pass

# /richard/_pico-tracker/firmware/test/tc/backlog_write_read.h: 32
for _lib in _libs.values():
    try:
        dp = (struct_tracker_datapoint).in_dll(_lib, 'dp')
        break
    except:
        pass

# /richard/_pico-tracker/firmware/test/tc/backlog_write_read.h: 33
for _lib in _libs.values():
    try:
        dp_ptr = (POINTER(struct_tracker_datapoint)).in_dll(_lib, 'dp_ptr')
        break
    except:
        pass

# /richard/_pico-tracker/firmware/test/tc/mem_write_all.h: 17
for _lib in _libs.values():
    try:
        mem_write_all_tc_params = (struct_mem_write_all_tc_params).in_dll(_lib, 'mem_write_all_tc_params')
        break
    except:
        pass

# /richard/_pico-tracker/firmware/test/tc/mem_write_all.h: 26
for _lib in _libs.values():
    try:
        mem_write_all_tc_results = (struct_mem_write_all_tc_results).in_dll(_lib, 'mem_write_all_tc_results')
        break
    except:
        pass

# /richard/_pico-tracker/firmware/test/tc/mem_write_all.h: 36
for _lib in _libs.values():
    try:
        page_read = (c_uint8 * 256).in_dll(_lib, 'page_read')
        break
    except:
        pass

# /richard/_pico-tracker/firmware/test/tc/mem_write_all.h: 37
for _lib in _libs.values():
    try:
        i = (c_uint32).in_dll(_lib, 'i')
        break
    except:
        pass

# /richard/_pico-tracker/firmware/test/tc/mem_write_all.h: 37
for _lib in _libs.values():
    try:
        j = (c_uint32).in_dll(_lib, 'j')
        break
    except:
        pass

# /richard/_pico-tracker/firmware/test/tc/times_two.h: 11
for _lib in _libs.values():
    try:
        times_two_tc_params = (struct_times_two_tc_params).in_dll(_lib, 'times_two_tc_params')
        break
    except:
        pass

# /richard/_pico-tracker/firmware/test/tc/times_two.h: 15
for _lib in _libs.values():
    try:
        times_two_tc_results = (struct_times_two_tc_results).in_dll(_lib, 'times_two_tc_results')
        break
    except:
        pass

# /richard/_pico-tracker/firmware/test/tc/mem_erase_all.h: 17
for _lib in _libs.values():
    try:
        mem_erase_all_tc_params = (struct_mem_erase_all_tc_params).in_dll(_lib, 'mem_erase_all_tc_params')
        break
    except:
        pass

# /richard/_pico-tracker/firmware/test/tc/mem_erase_all.h: 24
for _lib in _libs.values():
    try:
        mem_erase_all_tc_results = (struct_mem_erase_all_tc_results).in_dll(_lib, 'mem_erase_all_tc_results')
        break
    except:
        pass

# /richard/_pico-tracker/firmware/test/tc/location_telemetry.h: 15
for _lib in _libs.values():
    try:
        location_telemetry_tc_params = (struct_location_telemetry_tc_params).in_dll(_lib, 'location_telemetry_tc_params')
        break
    except:
        pass

# /richard/_pico-tracker/firmware/test/tc/location_telemetry.h: 19
for _lib in _libs.values():
    try:
        location_telemetry_tc_results = (struct_location_telemetry_tc_results).in_dll(_lib, 'location_telemetry_tc_results')
        break
    except:
        pass

# /richard/_pico-tracker/firmware/test/tc/pressure_temperature.h: 19
for _lib in _libs.values():
    try:
        pressure_temperature_tc_params = (struct_pressure_temperature_tc_params).in_dll(_lib, 'pressure_temperature_tc_params')
        break
    except:
        pass

# /richard/_pico-tracker/firmware/test/tc/pressure_temperature.h: 26
for _lib in _libs.values():
    try:
        pressure_temperature_tc_results = (struct_pressure_temperature_tc_results).in_dll(_lib, 'pressure_temperature_tc_results')
        break
    except:
        pass

# /richard/_pico-tracker/firmware/test/tc/gps_poll.h: 17
for _lib in _libs.values():
    try:
        gps_poll_tc_params = (struct_gps_poll_tc_params).in_dll(_lib, 'gps_poll_tc_params')
        break
    except:
        pass

# /richard/_pico-tracker/firmware/test/tc/gps_poll.h: 27
for _lib in _libs.values():
    try:
        gps_poll_tc_results = (struct_gps_poll_tc_results).in_dll(_lib, 'gps_poll_tc_results')
        break
    except:
        pass

# /richard/_pico-tracker/firmware/test/tc/gps_poll.h: 38
for _lib in _libs.values():
    try:
        data = (struct_gps_data_t).in_dll(_lib, 'data')
        break
    except:
        pass

# /richard/_pico-tracker/firmware/test/tc/epoch_from_time.h: 17
for _lib in _libs.values():
    try:
        epoch_from_time_tc_params = (struct_epoch_from_time_tc_params).in_dll(_lib, 'epoch_from_time_tc_params')
        break
    except:
        pass

# /richard/_pico-tracker/firmware/test/tc/epoch_from_time.h: 24
for _lib in _libs.values():
    try:
        epoch_from_time_tc_results = (struct_epoch_from_time_tc_results).in_dll(_lib, 'epoch_from_time_tc_results')
        break
    except:
        pass

# /richard/_pico-tracker/firmware/test/tc/epoch_from_time.h: 35
for _lib in _libs.values():
    try:
        t = (struct_tracker_time).in_dll(_lib, 't')
        break
    except:
        pass

# /richard/_pico-tracker/firmware/test/tc/gps_baud_error.h: 20
for _lib in _libs.values():
    try:
        gps_baud_error_tc_params = (struct_gps_baud_error_tc_params).in_dll(_lib, 'gps_baud_error_tc_params')
        break
    except:
        pass

# /richard/_pico-tracker/firmware/test/tc/gps_baud_error.h: 30
for _lib in _libs.values():
    try:
        gps_baud_error_tc_results = (struct_gps_baud_error_tc_results).in_dll(_lib, 'gps_baud_error_tc_results')
        break
    except:
        pass

# /richard/_pico-tracker/firmware/test/tc/gps_baud_error.h: 41
for _lib in _libs.values():
    try:
        sercom_index = (c_uint32).in_dll(_lib, 'sercom_index')
        break
    except:
        pass

# /richard/_pico-tracker/firmware/test/tc/gps_baud_error.h: 42
for _lib in _libs.values():
    try:
        gclk_index = (c_uint32).in_dll(_lib, 'gclk_index')
        break
    except:
        pass

# /richard/_pico-tracker/firmware/test/tc/gps_baud_error.h: 43
for _lib in _libs.values():
    try:
        baudrate = (c_uint32).in_dll(_lib, 'baudrate')
        break
    except:
        pass

# /richard/_pico-tracker/firmware/test/tc/gps_baud_error.h: 44
for _lib in _libs.values():
    try:
        baud = (c_uint16).in_dll(_lib, 'baud')
        break
    except:
        pass

# /richard/_pico-tracker/firmware/test/tc/gps_baud_error.h: 45
for _lib in _libs.values():
    try:
        calcuated_baud = (c_uint32).in_dll(_lib, 'calcuated_baud')
        break
    except:
        pass

# /richard/_pico-tracker/firmware/test/tc/crc32_gen_buf.h: 19
for _lib in _libs.values():
    try:
        crc32_gen_buf_tc_params = (struct_crc32_gen_buf_tc_params).in_dll(_lib, 'crc32_gen_buf_tc_params')
        break
    except:
        pass

# /richard/_pico-tracker/firmware/test/tc/crc32_gen_buf.h: 27
for _lib in _libs.values():
    try:
        crc32_gen_buf_tc_results = (struct_crc32_gen_buf_tc_results).in_dll(_lib, 'crc32_gen_buf_tc_results')
        break
    except:
        pass

# /richard/_pico-tracker/firmware/test/tc/crc32_gen_buf.h: 37
for _lib in _libs.values():
    try:
        test = (c_uint32).in_dll(_lib, 'test')
        break
    except:
        pass

# /richard/_pico-tracker/firmware/test/tc/crc32_gen_buf.h: 38
for _lib in _libs.values():
    try:
        uint8t_buffer = (POINTER(c_uint8)).in_dll(_lib, 'uint8t_buffer')
        break
    except:
        pass

# /richard/_pico-tracker/firmware/test/tc/analogue_read.h: 18
for _lib in _libs.values():
    try:
        analogue_read_tc_params = (struct_analogue_read_tc_params).in_dll(_lib, 'analogue_read_tc_params')
        break
    except:
        pass

# /richard/_pico-tracker/firmware/test/tc/analogue_read.h: 27
for _lib in _libs.values():
    try:
        analogue_read_tc_results = (struct_analogue_read_tc_results).in_dll(_lib, 'analogue_read_tc_results')
        break
    except:
        pass

# /richard/_pico-tracker/firmware/test/tc/read_rocket.h: 12
for _lib in _libs.values():
    try:
        buffer = (c_uint8 * 256).in_dll(_lib, 'buffer')
        break
    except:
        pass

# /richard/_pico-tracker/firmware/test/tc/read_rocket.h: 20
class struct_read_rocket_tc_params(Structure):
    pass

struct_read_rocket_tc_params.__slots__ = [
    'index',
]
struct_read_rocket_tc_params._fields_ = [
    ('index', c_uint32),
]

# /richard/_pico-tracker/firmware/test/tc/read_rocket.h: 20
for _lib in _libs.values():
    try:
        read_rocket_tc_params = (struct_read_rocket_tc_params).in_dll(_lib, 'read_rocket_tc_params')
        break
    except:
        pass

# /richard/_pico-tracker/firmware/test/tc/read_rocket.h: 27
class struct_read_rocket_tc_results(Structure):
    pass

struct_read_rocket_tc_results.__slots__ = [
    'dp',
]
struct_read_rocket_tc_results._fields_ = [
    ('dp', struct_tracker_datapoint),
]

# /richard/_pico-tracker/firmware/test/tc/read_rocket.h: 27
for _lib in _libs.values():
    try:
        read_rocket_tc_results = (struct_read_rocket_tc_results).in_dll(_lib, 'read_rocket_tc_results')
        break
    except:
        pass

# /richard/_pico-tracker/firmware/test/tc/read_rocket.h: 38
for _lib in _libs.values():
    try:
        index = (c_uint32).in_dll(_lib, 'index')
        break
    except:
        pass

# /richard/_pico-tracker/firmware/test/tc/adc_battery_solar_read.h: 17
for _lib in _libs.values():
    try:
        adc_battery_solar_read_tc_params = (struct_adc_battery_solar_read_tc_params).in_dll(_lib, 'adc_battery_solar_read_tc_params')
        break
    except:
        pass

# /richard/_pico-tracker/firmware/test/tc/adc_battery_solar_read.h: 25
for _lib in _libs.values():
    try:
        adc_battery_solar_read_tc_results = (struct_adc_battery_solar_read_tc_results).in_dll(_lib, 'adc_battery_solar_read_tc_results')
        break
    except:
        pass

# /richard/_pico-tracker/firmware/test/tc/location_aprs.h: 16
for _lib in _libs.values():
    try:
        location_aprs_tc_params = (struct_location_aprs_tc_params).in_dll(_lib, 'location_aprs_tc_params')
        break
    except:
        pass

# /richard/_pico-tracker/firmware/test/tc/location_aprs.h: 23
for _lib in _libs.values():
    try:
        location_aprs_tc_results = (struct_location_aprs_tc_results).in_dll(_lib, 'location_aprs_tc_results')
        break
    except:
        pass

# /richard/_pico-tracker/firmware/test/tc/backlog_read.h: 20
for _lib in _libs.values():
    try:
        backlog_read_tc_params = (struct_backlog_read_tc_params).in_dll(_lib, 'backlog_read_tc_params')
        break
    except:
        pass

# /richard/_pico-tracker/firmware/test/tc/backlog_read.h: 28
for _lib in _libs.values():
    try:
        backlog_read_tc_results = (struct_backlog_read_tc_results).in_dll(_lib, 'backlog_read_tc_results')
        break
    except:
        pass

# /richard/_pico-tracker/firmware/test/tc/backlog_read.h: 30
for _lib in _libs.values():
    try:
        backlog_index = (c_uint16).in_dll(_lib, 'backlog_index')
        break
    except:
        pass

# /richard/_pico-tracker/firmware/test/tc/backlog_read.h: 33
for _lib in _libs.values():
    try:
        dp_ptr = (POINTER(struct_tracker_datapoint)).in_dll(_lib, 'dp_ptr')
        break
    except:
        pass

# /richard/_pico-tracker/firmware/test/tc/thermistor_equation.h: 18
for _lib in _libs.values():
    try:
        thermistor_equation_tc_params = (struct_thermistor_equation_tc_params).in_dll(_lib, 'thermistor_equation_tc_params')
        break
    except:
        pass

# /richard/_pico-tracker/firmware/test/tc/thermistor_equation.h: 25
for _lib in _libs.values():
    try:
        thermistor_equation_tc_results = (struct_thermistor_equation_tc_results).in_dll(_lib, 'thermistor_equation_tc_results')
        break
    except:
        pass

# test/tc/crc32_gen_buf.h: 11
try:
    TEST_BUF_LEN = 32
except:
    pass

# /richard/_pico-tracker/firmware/test/tc/crc32_gen_buf.h: 11
try:
    TEST_BUF_LEN = 32
except:
    pass

times_two_tc_params = struct_times_two_tc_params # test/tc/times_two.h: 11

times_two_tc_results = struct_times_two_tc_results # test/tc/times_two.h: 15

osc8m_calib_tc_params = struct_osc8m_calib_tc_params # test/tc/osc8m_calib.h: 13

osc8m_calib_tc_results = struct_osc8m_calib_tc_results # test/tc/osc8m_calib.h: 19

location_aprs_tc_params = struct_location_aprs_tc_params # test/tc/location_aprs.h: 16

location_aprs_tc_results = struct_location_aprs_tc_results # test/tc/location_aprs.h: 23

location_telemetry_tc_params = struct_location_telemetry_tc_params # test/tc/location_telemetry.h: 15

location_telemetry_tc_results = struct_location_telemetry_tc_results # test/tc/location_telemetry.h: 19

mem_write_page_tc_params = struct_mem_write_page_tc_params # test/tc/mem_write_page.h: 18

mem_write_page_tc_results = struct_mem_write_page_tc_results # test/tc/mem_write_page.h: 25

mem_write_all_tc_params = struct_mem_write_all_tc_params # test/tc/mem_write_all.h: 17

mem_write_all_tc_results = struct_mem_write_all_tc_results # test/tc/mem_write_all.h: 26

location_aprs_file_tc_params = struct_location_aprs_file_tc_params # test/tc/location_aprs_file.h: 17

location_aprs_file_tc_results = struct_location_aprs_file_tc_results # test/tc/location_aprs_file.h: 25

backlog_write_read_tc_params = struct_backlog_write_read_tc_params # test/tc/backlog_write_read.h: 19

backlog_write_read_tc_results = struct_backlog_write_read_tc_results # test/tc/backlog_write_read.h: 27

backlog_read_tc_params = struct_backlog_read_tc_params # test/tc/backlog_read.h: 20

backlog_read_tc_results = struct_backlog_read_tc_results # test/tc/backlog_read.h: 28

mem_erase_all_tc_params = struct_mem_erase_all_tc_params # test/tc/mem_erase_all.h: 17

mem_erase_all_tc_results = struct_mem_erase_all_tc_results # test/tc/mem_erase_all.h: 24

analogue_read_tc_params = struct_analogue_read_tc_params # test/tc/analogue_read.h: 18

analogue_read_tc_results = struct_analogue_read_tc_results # test/tc/analogue_read.h: 27

adc_battery_solar_read_tc_params = struct_adc_battery_solar_read_tc_params # test/tc/adc_battery_solar_read.h: 17

adc_battery_solar_read_tc_results = struct_adc_battery_solar_read_tc_results # test/tc/adc_battery_solar_read.h: 25

gps_baud_error_tc_params = struct_gps_baud_error_tc_params # test/tc/gps_baud_error.h: 20

gps_baud_error_tc_results = struct_gps_baud_error_tc_results # test/tc/gps_baud_error.h: 30

crc32_gen_buf_tc_params = struct_crc32_gen_buf_tc_params # test/tc/crc32_gen_buf.h: 19

crc32_gen_buf_tc_results = struct_crc32_gen_buf_tc_results # test/tc/crc32_gen_buf.h: 27

barometric_altitude_tc_params = struct_barometric_altitude_tc_params # test/tc/barometric_altitude.h: 17

barometric_altitude_tc_results = struct_barometric_altitude_tc_results # test/tc/barometric_altitude.h: 24

pressure_temperature_tc_params = struct_pressure_temperature_tc_params # test/tc/pressure_temperature.h: 19

pressure_temperature_tc_results = struct_pressure_temperature_tc_results # test/tc/pressure_temperature.h: 26

thermistor_equation_tc_params = struct_thermistor_equation_tc_params # test/tc/thermistor_equation.h: 18

thermistor_equation_tc_results = struct_thermistor_equation_tc_results # test/tc/thermistor_equation.h: 25

gps_poll_tc_params = struct_gps_poll_tc_params # test/tc/gps_poll.h: 17

gps_poll_tc_results = struct_gps_poll_tc_results # test/tc/gps_poll.h: 27

epoch_from_time_tc_params = struct_epoch_from_time_tc_params # test/tc/epoch_from_time.h: 17

epoch_from_time_tc_results = struct_epoch_from_time_tc_results # test/tc/epoch_from_time.h: 24

read_rocket_tc_params = struct_read_rocket_tc_params # /richard/_pico-tracker/firmware/test/tc/read_rocket.h: 20

read_rocket_tc_results = struct_read_rocket_tc_results # /richard/_pico-tracker/firmware/test/tc/read_rocket.h: 27

# No inserted files

