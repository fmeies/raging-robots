ifndef SYSTEM
SYSTEM = $(shell uname)
endif

include $(RAGINGROBOTS_HOME)/config-$(SYSTEM)/config.dat

-include $(LOCAL_RAGINGROBOTS_HOME)/config.override

VERSION_MAJOR =	0
VERSION_MINOR =	3
PATCH_LEVEL =	7

VERSION =	$(VERSION_MAJOR).$(VERSION_MINOR).$(PATCH_LEVEL)

#ifndef LIB_DIR
#LIB_DIR =	-L$(RAGINGROBOTS_HOME)/lib/$(SYSTEM)
#endif
