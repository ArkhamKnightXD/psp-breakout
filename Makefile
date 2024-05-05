TARGET = TUTORIAL
OBJS = main.o

CFLAGS = 
CXXFLAGS = $(CFLAGS) -std=c++14 -fno-rti
ASFLAGS = $(CFLAGS)

#PSP STUFFS
BUILD_PRX = 1
PSP_FW_VERSION = 500
PSP_LARGE_MEMORY = 1

# PSP libraries
LIBS = -lpspgu -lpspge -lpspdisplay -lpspctrl

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = My First App
#PSP_EBOOT_ICON = ICON0.PNG

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
