.PHONY: all system-info help depend libs apps dist \
        install install-bin install-apps \
	uninstall uninstall-bin uninstall-apps

RAGINGROBOTS_HOME = .
include $(RAGINGROBOTS_HOME)/rules/init.mk

APP_DIRS =	ragingrobots rwserver 
SUB_DIRS =	$(APP_DIRS)

all: apps

help:
	@(\
	echo "make help            prints this help-text";\
	echo "make system-info     prints system-identifier";\
	echo "make all             = make apps";\
	echo "make depend          sets up dependencies";\
	echo "make apps            compiles the applications";\
	echo "make dist            tar and gzip everything to \$$TARGET_DIR";\
	echo "make install         = make install-bin install-data";\
	echo "make install-bin     installs all binaries";\
	echo "make uninstall       = make uninstall-bin uninstall-data";\
	echo "make uninstall-bin   removes all installed binaries"; \
	echo "make mostlyclean     deletes systems' programs and objects";\
	echo "make clean           deletes systems' binaries";\
	echo "make realclean       deletes all files created for this system";\
	echo "make distclean       deletes everything that can be remade";\
	)

system-info:
	@echo "$(SYSTEM)"

dist:
	rules/collect.sh \
	  --home=$(RAGINGROBOTS_HOME) \
	  --version=$(VERSION) \
	  --target-dir=$(TARGET_DIR)


apps:
	for DIR in $(APP_DIRS); do $(MAKE) -C $$DIR; done

include $(RAGINGROBOTS_HOME)/rules/subdirs.mk
