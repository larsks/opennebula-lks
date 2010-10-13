# -------------------------------------------------------------------------- #
# Copyright 2002-2010, OpenNebula Project Leads (OpenNebula.org)             #
#                                                                            #
# Licensed under the Apache License, Version 2.0 (the "License"); you may    #
# not use this file except in compliance with the License. You may obtain    #
# a copy of the License at                                                   #
#                                                                            #
# http://www.apache.org/licenses/LICENSE-2.0                                 #
#                                                                            #
# Unless required by applicable law or agreed to in writing, software        #
# distributed under the License is distributed on an "AS IS" BASIS,          #
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.   #
# See the License for the specific language governing permissions and        #
# limitations under the License.                                             #
#--------------------------------------------------------------------------- #

import os
import sys
import shutil
sys.path.append("./share/scons")
from lex_bison import *

# This is the absolute path where the project is located
cwd=os.getcwd()

# Environment that will be applied to each scons child
main_env=Environment()
main_env['ENV']['PATH']=os.environ['PATH']

# snippet borrowed from http://dev.gentoo.org/~vapier/scons-blows.txt
# makes scons aware of build related environment variables
if os.environ.has_key('CC'):
    main_env['CC'] = os.environ['CC']
if os.environ.has_key('CFLAGS'):
    main_env['CCFLAGS'] += SCons.Util.CLVar(os.environ['CFLAGS'])
if os.environ.has_key('CXX'):
    main_env['CXX'] = os.environ['CXX']
if os.environ.has_key('CXXFLAGS'):
    main_env['CXXFLAGS'] += SCons.Util.CLVar(os.environ['CXXFLAGS'])
if os.environ.has_key('LDFLAGS'):
    main_env['LINKFLAGS'] += SCons.Util.CLVar(os.environ['LDFLAGS'])

# Add builders for flex and bison
add_lex(main_env)
add_bison(main_env)

# Include dirs
main_env.Append(CPPPATH=[
    cwd+'/include',
])

# Library dirs
main_env.Append(LIBPATH=[
    cwd+'/src/common',
    cwd+'/src/log',
    cwd+'/src/sql',
    cwd+'/src/host',
    cwd+'/src/mad',
    cwd+'/src/nebula',
    cwd+'/src/pool',
    cwd+'/src/template',
    cwd+'/src/vm',
    cwd+'/src/vmm',
    cwd+'/src/lcm',
    cwd+'/src/tm',
    cwd+'/src/dm',
    cwd+'/src/im',
    cwd+'/src/image',
    cwd+'/src/rm',
    cwd+'/src/vnm',
    cwd+'/src/hm',
    cwd+'/src/um',
    cwd+'/src/authm',
])

# Compile flags
main_env.Append(CPPFLAGS=[
    "-g",
    "-Wall",
    "-Werror",
])

# Linking flags
main_env.Append(LINKFLAGS=['-g', '-pthread'])

#######################
# EXTRA CONFIGURATION #
#######################

# SQLITE
sqlite_dir=ARGUMENTS.get('sqlite_dir', 'none')
if sqlite_dir!='none':
    main_env.Append(LIBPATH=[sqlite_dir+"/lib"])
    main_env.Append(CPPPATH=[sqlite_dir+"/include"])

sqlite=ARGUMENTS.get('sqlite', 'yes')
if sqlite=='yes':
    main_env.Append(sqlite='yes')
    main_env.Append(CPPFLAGS=["-DSQLITE_DB"])
else:
    main_env.Append(sqlite='no')

# MySQL
mysql=ARGUMENTS.get('mysql', 'no')
if mysql=='yes':
    main_env.Append(mysql='yes')
    main_env.Append(CPPFLAGS=["-DMYSQL_DB"])
else:
    main_env.Append(mysql='no')


# xmlrpc
xmlrpc_dir=ARGUMENTS.get('xmlrpc', 'none')
if xmlrpc_dir!='none':
    main_env.Append(LIBPATH=[xmlrpc_dir+"/lib"])
    main_env.Append(CPPPATH=[xmlrpc_dir+"/include"])

# build lex/bison
build_parsers=ARGUMENTS.get('parsers', 'no')
if build_parsers=='yes':
    main_env.Append(parsers='yes')
else:
    main_env.Append(parsers='no')

if not main_env.GetOption('clean'):
    try:
        main_env.ParseConfig('share/scons/get_xmlrpc_config server')
        main_env.ParseConfig('share/scons/get_xmlrpc_config client')

        if mysql=='yes':
            main_env.ParseConfig('mysql_config --cflags --libs')

    except Exception, e:
        print ""
        print "Error searching for xmlrpc-c libraries. Please check this"+\
            " things:"
        print ""
        print " * You have installed development libraries for xmlrpc-c. One"+\
            " way to check"
        print "   this is calling xmlrpc-c-config that is provided with the"+\
            " development"
        print "   package."
        print " * Check that the version of xmlrpc-c is at least 1.06. You"+\
            " can do this also"
        print "   calling:"
        print "   $ xmlrpc-c-config --version"
        print " * If all this requirements are already met please send log"+\
            " files located in"
        print "   .xmlrpc_test to the mailing list."
        print ""
        exit(-1)
else:
    main_env.Replace(mysql='yes')
    shutil.rmtree('.xmlrpc_test', True)
    shutil.rmtree('src/nebula/.xmlrpc_test', True)
    shutil.rmtree('src/scheduler/.xmlrpc_test', True)


# libxml2
main_env.ParseConfig('xml2-config --libs --cflags')


# SCONS scripts to build
build_scripts=[
    'src/sql/SConstruct',
    'src/log/SConstruct',
    'src/common/SConstruct',
    'src/template/SConstruct',
    'src/host/SConstruct',
    'src/mad/SConstruct',
    'src/nebula/SConstruct',
    'src/pool/SConstruct',
    'src/vm/SConstruct',
    'src/vmm/SConstruct',
    'src/lcm/SConstruct',
    'src/rm/SConstruct',
    'src/tm/SConstruct',
    'src/im/SConstruct',
    'src/image/SConstruct',
    'src/dm/SConstruct',
    'src/scheduler/SConstruct',
    'src/vnm/SConstruct',
    'src/hm/SConstruct',
    'src/um/SConstruct',
    'src/authm/SConstruct',
]

for script in build_scripts:
    env=main_env.Clone()
    SConscript(script, exports='env')


