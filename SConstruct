# -*- mode: python -*-

import os

def install_under(top_dir, env, feature):
    for (dir, file) in feature.iteritems():
        installed = env.Install(os.path.join(top_dir, dir), file)

###################################
#### Configuration        

prefix = "#/install"

        
blank_env = Environment()
Export("blank_env")

#blank_env["CXX"] = "/s/gcc-4.6.1/bin/g++"
blank_env.Append(CXXFLAGS=["-std=c++0x"])
#blank_env.Append(CPPPATH=["/unsup/boost-1.49.0/include"])
#blank_env.Append(LIBPATH=["/unsup/boost-1.49.0/lib"])
#blank_env.Append(RPATH=["/unsup/boost-1.49.0/lib"])
#blank_env.Append(LIBS=["boost_system", "boost_chrono", "boost_thread"])
blank_env.Append(LIBS=["boost_thread"])
blank_env.Append(CCFLAGS=["-g"])

###################################
#### Build getmemusage.so

getmemusage = blank_env.SConscript("getmemusage/src/SConscript")

install_under(prefix, blank_env, getmemusage)

#### Build getmemusage tests

env_link_to_getmemusage = blank_env.Clone()
Export("env_link_to_getmemusage")

env_link_to_getmemusage.Append(LIBS = ["getmemusage", "rt"])
env_link_to_getmemusage.Append(LIBPATH = [os.path.join(prefix, "lib")])
env_link_to_getmemusage.Append(RPATH_TRANSFORM = [os.path.join(prefix, "lib")])
env_link_to_getmemusage.Append(CPPPATH = [os.path.join(prefix, "include")])

env_link_to_getmemusage.SConscript("getmemusage/tests/SConscript")

###################################
#### Build monitor.so

monitor = env_link_to_getmemusage.SConscript("monitor/src/SConscript")

install_under(prefix, env_link_to_getmemusage, monitor)

#### Build tests

env_link_to_monitor = env_link_to_getmemusage.Clone()
env_link_to_monitor.Append(LIBS=["monitor"])

env_link_to_monitor.Program("dumb", "monitor/src/program.cpp")
