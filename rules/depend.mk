.PHONY: depend

# hide local-ragingrobots_home in &
ifdef LOCAL_RAGINGROBOTS_HOME
POSTPROCESS += -e "s@$(subst .,\.,$(LOCAL_RAGINGROBOTS_HOME))@\&@g"
endif

# hide ragingrobots-home in %
POSTPROCESS = -e "s@$(subst .,\.,$(RAGINGROBOTS_HOME))@\%@g"

# remove remaining absolute pathes
POSTPROCESS += -e "s@ /[-+_./a-zA-Z0-9]*@@g"

# remove empty rules
POSTPROCESS += -e "s@^.*: *\$$@@"

# exchange the target - add the directory etc.
POSTPROCESS += -e "s@[^ ].*:@$$\(SYSTEM\)\/&@"

# expand % to the ragingrobots-home macro
POSTPROCESS += -e "s@\%@$$\(RAGINGROBOTS_HOME\)@g"

# expand % to the local-ragingrobots-home macro
ifdef LOCAL_RAGINGROBOTS_HOME
POSTPROCESS += -e "s@\&@$$\(LOCAL_RAGINGROBOTS_HOME\)@g"
endif

# smart style - everything automatically
depend:
	rm -f .depend

ifneq (,$(CXXFILES))
ifeq ($(CXX_HAS_M),YES)
	$(CXX) -M $(ALL_CXXFLAGS) $(CXXFILES) >.depend.tmp
	sed $(POSTPROCESS) <.depend.tmp >.depend
else
ifneq (,$(MAKEDEPEND))
	rm -f .depend.tmp; touch .depend.tmp
	makedepend -D__cplusplus -f .depend.tmp $(INC_FLAGS) $(CXXFILES) \
	  2>.depend.err
	sed $(POSTPROCESS) \
	  <.depend.tmp >.depend.tmp2
	eval sed `grep "not in [a-z]" < .depend.err | \
	  sed -e "s@.*not in @@" -e "s@^@-e \"s\@@" -e "s@\$$@\@\@g\"@" |
	  tr "\012" " "` < .depend.tmp2 >.depend
	$(RAGINGROBOTS_HOME)/scripts/fixdepend
else
	@echo "can't detect dependencies"
	@false
endif # MAKEDEPEND
endif # CXX_HAS_M
endif # CXXFILES
	rm -f .depend.tmp* .depend.err

-include .depend
