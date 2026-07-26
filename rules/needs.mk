MESA_LIBS = -lMesaGL
OPENGL_LIBS = -lGL
GLU_LIBS = -lGLU
PLIB_LIBS = -lplibsl -lplibul
GLUT_LIBS = -lglut

ifeq ($(HAVE_OPENGL),NO)
  ifeq ($(HAVE_MESA),YES)
    override HAVE_OPENGL = $(HAVE_MESA)
    override OPENGL_ALTERNATIVE = -DHAVE_MESA
    override OPENGL_HOME = $(MESA_HOME)
    override OPENGL_LIBS = $(MESA_LIBS)
  endif
endif

RAGINGROBOTS_LIBS += $(LIB_DIR)

ifeq ($(HAVE_GLU),YES)
  ifneq (,$(findstring GLU,$(LIKES)))
    ifeq (,$(findstring GLU,$(NEEDS)))
      NEEDS += GLU
    endif
  endif
endif

ifneq (,$(findstring GLU,$(NEEDS)))
ifneq ($(HAVE_GLU),YES)
ifneq ($(FAILURE),YES)
$(TARGET):
	@echo "glu not available"
	@false
FAILURE=YES
endif
endif
ifeq (,$(findstring X11,$(NEEDS)))
  NEEDS += X11
endif
EXTERNAL_FLAGS += -DHAVE_GLU
ifneq ($(GLU_HOME),/usr)
ifeq (,$(findstring $(GLU_HOME)/include, $(EXTERNAL_INCLUDES)))
  EXTERNAL_INCLUDES += -I$(GLU_HOME)/include
  EXTERNAL_LIBS += -L$(GLU_HOME)/lib
endif
endif
EXTERNAL_LIBS += $(GLU_LIBS)
endif

ifeq ($(HAVE_OPENGL),YES)
  ifneq (,$(findstring OPENGL,$(LIKES)))
    ifeq (,$(findstring OPENGL,$(NEEDS)))
      NEEDS += OPENGL
    endif
  endif
endif

ifneq (,$(findstring OPENGL,$(NEEDS)))
ifneq ($(HAVE_OPENGL),YES)
ifneq ($(FAILURE),YES)
$(TARGET):
	@echo "opengl not available"
	@false
FAILURE=YES
endif
endif
ifeq (,$(findstring X11,$(NEEDS)))
  NEEDS += X11
endif
EXTERNAL_FLAGS += $(OPENGL_ALTERNATIVE) -DHAVE_OPENGL
ifneq ($(OPENGL_HOME),/usr)
ifeq (,$(findstring $(OPENGL_HOME)/include, $(EXTERNAL_INCLUDES)))
  EXTERNAL_INCLUDES += -I$(OPENGL_HOME)/include
  EXTERNAL_LIBS += -L$(OPENGL_HOME)/lib
endif
endif
EXTERNAL_LIBS += $(OPENGL_LIBS)
endif

ifeq ($(HAVE_X11),YES)
  ifneq (,$(findstring X11,$(LIKES)))
    ifeq (,$(findstring X11,$(NEEDS)))
      NEEDS += X11
    endif
  endif
endif

ifneq (,$(findstring X11,$(NEEDS)))
ifneq ($(HAVE_X11),YES)
ifneq ($(FAILURE),YES)
$(TARGET):
	@echo "x11 not available"
	@false
FAILURE=YES
endif
endif
ifeq (,$(findstring NET,$(NEEDS)))
  NEEDS += NET
endif
EXTERNAL_FLAGS += -DHAVE_X11
ifneq ($(X11_HOME),/usr)
ifeq (,$(findstring $(X11_HOME)/include, $(EXTERNAL_INCLUDES)))
  EXTERNAL_INCLUDES += -I$(X11_HOME)/include
  EXTERNAL_LIBS += -L$(X11_HOME)/lib
endif
endif
EXTERNAL_LIBS += $(X11_LIBS)
endif

ifeq ($(HAVE_NET),YES)
  ifneq (,$(findstring NET,$(LIKES)))
    ifeq (,$(findstring NET,$(NEEDS)))
      NEEDS += NET
    endif
  endif
endif

ifneq (,$(findstring NET,$(NEEDS)))
ifneq ($(HAVE_NET),YES)
ifneq ($(FAILURE),YES)
$(TARGET):
	@echo "net not available"
	@false
FAILURE=YES
endif
endif
EXTERNAL_FLAGS += -DHAVE_NET
ifneq ($(NET_HOME),/usr)
ifeq (,$(findstring $(NET_HOME)/include, $(EXTERNAL_INCLUDES)))
  EXTERNAL_INCLUDES += -I$(NET_HOME)/include
  EXTERNAL_LIBS += -L$(NET_HOME)/lib
endif
endif
EXTERNAL_LIBS += $(NET_LIBS)
endif

ifeq ($(HAVE_GLUT),YES)
  ifneq (,$(findstring GLUT,$(LIKES)))
    ifeq (,$(findstring GLUT,$(NEEDS)))
      NEEDS += GLUT
    endif
  endif
endif

ifneq (,$(findstring GLUT,$(NEEDS)))
ifneq ($(HAVE_GLUT),YES)
ifneq ($(FAILURE),YES)
$(TARGET):
	@echo "glut not available"
	@false
FAILURE=YES
endif
endif
EXTERNAL_FLAGS += -DHAVE_GLUT
ifneq ($(GLUT_HOME),/usr/X11)
ifeq (,$(findstring $(GLUT_HOME)/include, $(EXTERNAL_INCLUDES)))
  EXTERNAL_INCLUDES += -I$(GLUT_HOME)/include
  EXTERNAL_LIBS += -L$(GLUT_HOME)/lib
endif
endif
EXTERNAL_LIBS += $(GLUT_LIBS)
endif

ifeq ($(HAVE_PLIB),YES)
  ifneq (,$(findstring PLIB,$(LIKES)))
    ifeq (,$(findstring PLIB,$(NEEDS)))
      NEEDS += PLIB
    endif
  endif
endif

ifneq (,$(findstring PLIB,$(NEEDS)))
ifneq ($(HAVE_PLIB),YES)
ifneq ($(FAILURE),YES)
$(TARGET):
	@echo "plib not available"
	@false
FAILURE=YES
endif
endif
EXTERNAL_FLAGS += -DHAVE_PLIB
ifneq ($(PLIB_HOME),/usr)
ifeq (,$(findstring $(PLIB_HOME)/include, $(EXTERNAL_INCLUDES)))
  EXTERNAL_INCLUDES += -I$(PLIB_HOME)/include
  EXTERNAL_LIBS += -L$(PLIB_HOME)/lib
endif
endif
EXTERNAL_LIBS += $(PLIB_LIBS)
endif

