# Expects the following variables to be set:
#   MODE: ANDROID or MAIN
#   GEN: 2, 3 or empty
#   PLATFORMDIR: path to this file
#   BUILDDIR: path to the build directory
#   CSOURCES: .c files to compile
#   CPPSOURCES: .cpp files to compile

include $(PLATFORMDIR)/vars.mk

FLAGS = $(DEFS) -I. -I$(PLATFORMDIR) $(WFLAGS) -MMD -MP -fpic -march=armv5te -Os -ffunction-sections -fdata-sections
CFLAGS = $(FLAGS) -std=c11
CPPFLAGS = $(FLAGS) -std=c++11 -Wno-vla
LDFLAGS = -Wl,--no-undefined $(LFLAGS) -Wl,-gc-sections

OBJS = $(SOURCES:%=$(BUILDDIR)/%.o)
LIBOBJS = $(LIBS:%=$(BUILDDIR)/$(LIBDIR)/lib%.so)

$(BUILDDIR)/$(LIBDIR)/lib%.so: $(BUILDDIR)/$(PLATFORMDIR)/$(DRIVERDIR)/%.c.o
	@mkdir -p $(dir $@)
	$(CC) -shared $< -o $@

$(BUILDDIR)/%.c.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILDDIR)/%.cpp.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) -c $< -o $@

-include $(OBJS:%.o=%.d)
