.PHONY:

OBJECTS =
OBJECTS +=	$(patsubst %.cpp,$(SYSTEM)/%.o,$(CXXFILES))

ifneq ($(USE_SHARED),YES)
override CXX_SO_FLAGS =
endif

INC_FLAGS =

# set include pathes
ifdef LOCAL_RAGINGROBOTS_HOME
INC_FLAGS +=	-I$(LOCAL_RAGINGROBOTS_HOME)/include
endif
INC_FLAGS +=	-I$(RAGINGROBOTS_HOME)/include
INC_FLAGS +=	$(foreach X,$(ADD_INC_DIR),-I$(X))
INC_FLAGS +=	$(EXTERNAL_INCLUDES)

# compose the flags
ALL_CXXFLAGS =	$(INC_FLAGS) $(CXX_SO_FLAGS) $(CXX_SYS_FLAGS) $(CXX_WARN_FLAGS) \
		$(EXTERNAL_FLAGS) $(RAGINGROBOTS_FLAGS) $(CXX_COMPAT_FLAGS) \
		$(CXXFLAGS) $(ADD_CXXFLAGS)

.PRECIOUS: $(SYSTEM)/.system depend/.system


$(SYSTEM)/%.o: %.cpp $(SYSTEM)/.system
	$(CXX) $(ALL_CXXFLAGS) -o $@ -c $<

%/.system:
	mkdir -p $*
	touch $@
