# Expects the following variables to be set:
#   MODE: ANDROID, MAIN or UPDATER
#   GEN: 1, 2, 3 or empty
#   PLATFORMDIR: path to this file
#   BUILDDIR: path to the build directory
#   CSOURCES: .c files to compile
#   CPPSOURCES: .cpp files to compile

include $(PLATFORMDIR)/vars.mk

FLAGS = $(DEFS) -I. -I$(PLATFORMDIR) $(WFLAGS) -MMD -MP -fpic -march=armv5te -Os -ffunction-sections -fdata-sections
CFLAGS = $(FLAGS) -std=c11
CPPFLAGS = $(FLAGS) -std=c++11 -Wno-vla
LDFLAGS = -Wl,--no-undefined $(LFLAGS) -Wl,--version-script=$(PLATFORMDIR)/updater/exportmap.txt -Wl,-gc-sections

OBJS = $(SOURCES:%=$(BUILDDIR)/%.o)
LIBOBJS = $(LIBS:%=$(BUILDDIR)/$(LIBDIR)/lib%.so)

$(BUILDDIR)/$(LIBDIR)/lib%.so: $(BUILDDIR)/$(PLATFORMDIR)/$(DRIVERDIR)/%.o
	@mkdir -p $(dir $@)
	$(CC) -shared $< -o $@

$(BUILDDIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILDDIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) -c $< -o $@

# On gen1 / gen2 devices, Sony use their own standard library. We'll just take
# GCC's libraries and rename them.
# Dirty hack: The SONAME has to be removed, too. We use sed to replace it with
# an empty string.
$(BUILDDIR)/$(LIBDIR)/libsony%.so:
	@mkdir -p $(dir $@)
	@cp "`$(CC) -print-file-name=lib$*.so`" $@ 2>/dev/null | true
	@cp "`$(CC) -print-file-name=lib$*.so.1`" $@ 2>/dev/null | true
	@cp "`$(CC) -print-file-name=lib$*-2.13.so`" $@ 2>/dev/null | true
	sed -i -b -e 's/lib$*.so/\x00ib$*.so/g' $@

# libsupc++ is not available as a shared library, build it from libsupc++.a
$(BUILDDIR)/$(LIBDIR)/libsonysupc++.so:
	$(CC) -nostdlib -Wl,--whole-archive -lsupc++ -shared -o $@

-include $(OBJS:%.o=%.d)
