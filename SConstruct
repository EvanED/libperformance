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

blank_env["CXX"] = "/s/gcc-4.6.1/bin/g++"
blank_env.Append(CXXFLAGS=["-std=gnu++0x"])
blank_env.Append(CPPPATH=["/unsup/boost-1.47.0/include"])

###################################
#### Build getmemusage.so

getmemusage = blank_env.SConscript("getmemusage/src/SConscript")

install_under(prefix, blank_env, getmemusage)

#### Build getmemusage tests

env_link_to_getmemusage = blank_env.Clone()
Export("env_link_to_getmemusage")

env_link_to_getmemusage.Append(LIBS = ["getmemusage"])
env_link_to_getmemusage.Append(LIBPATH = [os.path.join(prefix, "lib")])
env_link_to_getmemusage.Append(RPATH_TRANSFORM = [os.path.join(prefix, "lib")])
env_link_to_getmemusage.Append(CPPPATH = [os.path.join(prefix, "include")])

env_link_to_getmemusage.SConscript("getmemusage/tests/SConscript")

