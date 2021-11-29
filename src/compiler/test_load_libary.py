# -*- coding: utf-8 -*-

import os
import ctypes

path_lib = os.path.join(os.path.curdir + os.path.sep, "testlib.so")
print(path_lib)
lib = ctypes.CDLL(path_lib)
lib.main2(b"./gera.obj")