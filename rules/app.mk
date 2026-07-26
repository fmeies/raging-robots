.PHONY: all $(NAME) apps install uninstall

TARGET=$(SYSTEM)/$(NAME)

include $(RAGINGROBOTS_HOME)/rules/init.mk
include $(RAGINGROBOTS_HOME)/rules/compile.mk
include $(RAGINGROBOTS_HOME)/rules/needs.mk

LIBS =          $(RAGINGROBOTS_LIB_DIRS) $(ADD_RAGINGROBOTS_LIB) $(RAGINGROBOTS_LIBS) \
                $(ADD_EXTERNAL_LIB) $(EXTERNAL_LIBS)

TEMPLATE_OBJECTS = $(wildcard $(TEMPLATES))


ifndef CXX_LD
CXX_LD = $(CXX)
endif

ifneq ($(FAILURE),YES)
$(TARGET): $(ADD_TARGET) $(OBJECTS)
	$(CXX_LD) -o $@ $(OBJECTS) $(TEMPLATES) $(LIB_FLAGS) $(LIBS)
endif

install: install-bin install-data

install-bin: $(ADD_INSTALL) $(TARGET)
	$(INSTALL) -d $(PREFIX)/bin
	$(INSTALL) -s $(SYSTEM)/$(NAME) $(PREFIX)/bin

install-data: $(ADD_INSTALL) $(TARGET)
	if test -d ./data ; then \
	mkdir $(PREFIX)/share/$(NAME); \
	cp ./data/* $(PREFIX)/share/$(NAME); \
	fi

uninstall: uninstall-bin uninstall-data

uninstall-bin: $(ADD_UNINSTALL)
	rm -f $(PREFIX)/bin/$(NAME)

uninstall-data: $(ADD_UNINSTALL)
	rm -rf $(PREFIX)/share/$(NAME)

all $(NAME) apps: $(TARGET)

include $(RAGINGROBOTS_HOME)/rules/depend.mk
include $(RAGINGROBOTS_HOME)/rules/clean.mk
