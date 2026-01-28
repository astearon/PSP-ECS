TARGET = PSP-ECS
OBJS = src/main.o src/ecs.o src/menu.o src/keybinds.o src/scene.o src/camera.o

INCDIR = include
PSPSDK := $(shell psp-config --pspsdk-path)

INCDIR  := include .
INCDIR  += $(PSPSDK)/include

CFLAGS   = -O2 -G0 -Wall $(addprefix -I,$(INCDIR))
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS  = $(CFLAGS)

LIBDIR =
LDFLAGS =
LIBS = -lraylib -lGL -lpspvfpu -lm -lpspgu -lpspge -lpspdisplay -lpspctrl -lpspaudio -lpsppower -lpsputility

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = PSP-ECS Demo
PSP_EBOOT_ICON = NULL
PSP_EBOOT_PIC1 = NULL

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
