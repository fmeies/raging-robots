.PHONY: clean mostlyclean realclean distclean

include $(RAGINGROBOTS_HOME)/rules/init.mk

mostlyclean clean realclean distclean:
	$(RAGINGROBOTS_HOME)/rules/clean.sh --system=$(SYSTEM) --mode=$@ --home=$(RAGINGROBOTS_HOME)
