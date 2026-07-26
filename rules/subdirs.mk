depend install install-bin uninstall uninstall-bin:
	for X in $(SUB_DIRS); do $(MAKE) -C $$X $@ || exit 1; done

include $(RAGINGROBOTS_HOME)/rules/clean.mk
