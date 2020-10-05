#
# Copyright (c) 2001 - 2015 The SCons Foundation
#
# Permission is hereby granted, free of charge, to any person obtaining
# a copy of this software and associated documentation files (the
# "Software"), to deal in the Software without restriction, including
# without limitation the rights to use, copy, modify, merge, publish,
# distribute, sublicense, and/or sell copies of the Software, and to
# permit persons to whom the Software is furnished to do so, subject to
# the following conditions:
#
# The above copyright notice and this permission notice shall be included
# in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY
# KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
# WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
# NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
# LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
# OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
# WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#

"""SCons.Tool.cc

Tool-specific initialization for generic Posix C compilers.

There normally shouldn't be any need to import this module directly.
It will usually be imported through the generic SCons.Tool.Tool()
selection method.
"""


__revision__ = "src/engine/SCons/Tool/cc.py rel_2.4.1:3453:73fefd3ea0b0 2015/11/09 03:25:05 bdbaddog"

import SCons.Tool
import SCons.Defaults
import SCons.Util
import os

CSuffixes = ['.c', '.C', '.cxx', '.cpp', '.c++', '.cc', '.h', '.H', '.hxx', '.hpp', '.hh', '.m', '.mm', '.S', '.spp', '.SPP', '.sx']

if not SCons.Util.case_sensitive_suffixes('.c', '.C'):
    CSuffixes.append('.C')

def add_common_cc_variables(env):
    """
    Add underlying common "C compiler" variables that
    are used by multiple tools (specifically, c++).
    """
    if '_CCCOMCOM' not in env:
        env['_CCCOMCOM'] = '$CPPFLAGS $_CPPDEFFLAGS $_CPPINCFLAGS'
        # It's a hack to test for darwin here, but the alternative
        # of creating an applecc.py to contain this seems overkill.
        # Maybe someday the Apple platform will require more setup and
        # this logic will be moved.
        env['FRAMEWORKS'] = SCons.Util.CLVar('')
        env['FRAMEWORKPATH'] = SCons.Util.CLVar('')
        if env['PLATFORM'] == 'darwin':
            env['_CCCOMCOM'] = env['_CCCOMCOM'] + ' $_FRAMEWORKPATH'

    if 'CCFLAGS' not in env:
        env['CCFLAGS']   = SCons.Util.CLVar('')

    if 'SHCCFLAGS' not in env:
        env['SHCCFLAGS'] = SCons.Util.CLVar('$CCFLAGS')

compilers = ['cc']

def generate(env):
    """
    Add Builders and construction variables for C compilers to an Environment.
    """
    static_obj, shared_obj = SCons.Tool.createObjBuilders(env)

    for suffix in CSuffixes:
        static_obj.add_action(suffix, SCons.Defaults.CAction)
        shared_obj.add_action(suffix, SCons.Defaults.ShCAction)
        static_obj.add_emitter(suffix, SCons.Defaults.StaticObjectEmitter)
        shared_obj.add_emitter(suffix, SCons.Defaults.SharedObjectEmitter)

    add_common_cc_variables(env)

    # Tool c compiler execution
    env['CC_BIN']      = "clang"
    # temporary location
    # telling the tools which toolchains we are using. by default it is empty so GCC tools will be used
    env['STR_TOOL_PREFIX'] = ""
    env['SIZE_BIN']     = "riscv32-unknown-elf-size"
    env['OBJDUMP_BIN']  = "riscv32-unknown-elf-objdump"
    env['OBJDUMP_ATTR'] = ""
    env['OBJCOPY_BIN']  = "riscv32-unknown-elf-objcopy"
    env['READELF_BIN']  = "riscv32-unknown-elf-readelf"

    # llvm objdump tool 
    env['LLVM_OBJDUMP_BIN']  = "llvm-objdump"
    env['LLVM_OBJDUMP_ATTR'] = " --mattr=+experimental-b"
    
    env['CC'] = os.path.join("$RISCV_LLVM_TC_PATH", "bin", "$CC_BIN")
    env['CFLAGS'] = SCons.Util.CLVar('')
    env['CCCOM'] = '$CC -o $TARGET -c $CFLAGS $CCFLAGS $_CCCOMCOM $SOURCES $CCPATHS'
    env['SHCC'] = '$CC'
    env['SHCFLAGS'] = SCons.Util.CLVar('$CFLAGS')
    env['SHCCCOM'] = '$SHCC -o $TARGET -c $SHCFLAGS $SHCCFLAGS $_CCCOMCOM $SOURCES'
    
    env['LINKCOM'] = '$LINK -o $TARGET $LINKFLAGS $LINKCOM_START_GROUP $SOURCES $__RPATH $_LIBDIRFLAGS $_LIBFLAGS $LINKCOM_END_GROUP'
    
    env['CPPDEFPREFIX'] = '-D'
    env['CPPDEFSUFFIX'] = ''
    env['INCPREFIX'] = '-I'
    env['INCSUFFIX'] = ''
    env['SHOBJSUFFIX'] = '.os'
    env['STATIC_AND_SHARED_OBJECTS_ARE_THE_SAME'] = 0
    env['CFILESUFFIX'] = '.c'
    env['TOOLCHAIN'] = 'RISCV'

def exists(env):
    return env.Detect(env.get('CC', compilers))

# Local Variables:
# tab-width:4
# indent-tabs-mode:nil
# End:
# vim: set expandtab tabstop=4 shiftwidth=4:
