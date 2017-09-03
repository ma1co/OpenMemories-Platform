TARGET_OS := arm-linux-gnueabi
include $(SRCROOT)/Makefiles/gmake/sysid.mak
OSNAME := linux

_STATIC_BUILD := 1
_NO_SHARED_BUILD := 1
_NO_DBG_BUILD := 1
_NO_STLDBG_BUILD := 1
EXTRA_CFLAGS := -march=armv5te
EXTRA_CXXFLAGS := -march=armv5te
