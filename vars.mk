# Apis are high level code
APIDIR = api

# Drivers are low level code
DRIVERDIR = drivers

# Libs are drivers which are already present as .so files on the device
LIBDIR = libs


# Functions available in all modes:
LIBS_GEN_ = osal_uipc

# Functions available in MAIN mode:
LIBS_MAIN_GEN_ = backup

# Functions available in UPDATER mode:
LIBS_UPDATER_GEN_ = backup

# Functions available in ANDROID mode:
APIS_ANDROID_GEN_ = shell
DRIVERS_ANDROID_GEN_ = backup


# Collect available functions:
APIS = $(APIS_GEN_) $(APIS_GEN$(GEN)) $(APIS_$(MODE)_GEN_) $(APIS_$(MODE)_GEN$(GEN))
DRIVERS = $(DRIVERS_GEN_) $(DRIVERS_GEN$(GEN)) $(DRIVERS_$(MODE)_GEN_) $(DRIVERS_$(MODE)_GEN$(GEN))
LIBS = $(LIBS_GEN_) $(LIBS_GEN$(GEN)) $(LIBS_$(MODE)_GEN_) $(LIBS_$(MODE)_GEN$(GEN))

# All source files in the main executable
SOURCES = $(APIS:%=$(PLATFORMDIR)/$(APIDIR)/%.c) $(DRIVERS:%=$(PLATFORMDIR)/$(DRIVERDIR)/%.c) $(CSOURCES) $(CPPOURCES)


# GCC:
# Get it here: https://launchpad.net/linaro-toolchain-binaries/+milestone/2012.04
CC = arm-linux-gnueabi-gcc
CXX = arm-linux-gnueabi-g++

# Compiler flags
DEFS = -DMODE_$(MODE) -DGEN$(GEN) $(APIS:%=-DAPI_%) $(DRIVERS:%=-DDRIVER_%) $(LIBS:%=-DDRIVER_%)
WFLAGS = -Wall -Werror -Wundef -pedantic
LFLAGS = -s
