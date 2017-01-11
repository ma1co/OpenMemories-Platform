# Apis are high level code
APIDIR = api

# Drivers are low level code
DRIVERDIR = drivers

# Libs are drivers which are already present as .so files on the device
LIBDIR = libs


# Functions available in all modes:
DRIVERS_GEN_ = mem
DRIVERS_GEN2 = backup_senser
LIBS_GEN2 = osal_uipc
DRIVERS_GEN3 = backup_senser
LIBS_GEN3 = osal_uipc

# Functions available in MAIN mode:
APIS_MAIN_GEN2 = bootloader
APIS_MAIN_GEN3 = bootloader
DRIVERS_MAIN_GEN2 = mms_ioc
DRIVERS_MAIN_GEN3 = nand_ioc
LIBS_MAIN_GEN2 = backup
LIBS_MAIN_GEN3 = backup

# Functions available in UPDATER mode:
APIS_UPDATER_GEN1 = bootloader
APIS_UPDATER_GEN2 = bootloader
APIS_UPDATER_GEN3 = bootloader
DRIVERS_UPDATER_GEN1 = mms_ioc
DRIVERS_UPDATER_GEN2 = mms_ioc
DRIVERS_UPDATER_GEN3 = nand_ioc
LIBS_UPDATER_GEN1 = $(STDLIBS_GEN1:%=sony%)
LIBS_UPDATER_GEN2 = $(STDLIBS_GEN2:%=sony%) backup
LIBS_UPDATER_GEN3 = backup
LFLAGS_UPDATER_GEN1 = -nostdlib
LFLAGS_UPDATER_GEN2 = -nostdlib

# Functions available in ANDROID mode:
APIS_ANDROID_GEN_ = shell
DRIVERS_ANDROID_GEN_ = backup backup_senser
LIBS_ANDROID_GEN_ = osal_uipc


# Collect available functions:
APIS = $(sort $(APIS_GEN_) $(APIS_GEN$(GEN)) $(APIS_$(MODE)_GEN_) $(APIS_$(MODE)_GEN$(GEN)))
DRIVERS = $(sort $(DRIVERS_GEN_) $(DRIVERS_GEN$(GEN)) $(DRIVERS_$(MODE)_GEN_) $(DRIVERS_$(MODE)_GEN$(GEN)))
LIBS = $(sort $(LIBS_GEN_) $(LIBS_GEN$(GEN)) $(LIBS_$(MODE)_GEN_) $(LIBS_$(MODE)_GEN$(GEN)))

# Sony standard libraries on gen1 / gen2 devices
STDLIBS_GEN1 = c crypt dl gcc_s m pthread rt supc++
STDLIBS_GEN2 = c crypt dl gcc_s m pthread rt stdc++

# All source files in the main executable
SOURCES = $(APIS:%=$(PLATFORMDIR)/$(APIDIR)/%.c) $(DRIVERS:%=$(PLATFORMDIR)/$(DRIVERDIR)/%.c) $(CSOURCES) $(CPPOURCES)


# GCC:
# Get it here: https://launchpad.net/linaro-toolchain-binaries/+milestone/2012.04
CC = arm-linux-gnueabi-gcc
CXX = arm-linux-gnueabi-g++

# Compiler flags
DEFS = -DMODE_$(MODE) -DGEN$(GEN) $(APIS:%=-DAPI_%) $(DRIVERS:%=-DDRIVER_%) $(subst +,p,$(LIBS:%=-DDRIVER_%))
WFLAGS = -Wall -Werror -Wundef -pedantic
LFLAGS = -s $(LFLAGS_$(MODE)_GEN$(GEN))
